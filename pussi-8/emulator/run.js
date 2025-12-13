import specs from "./specs.js";
import { getInstructionName } from "./instructions.js";

let state, ui;

let interval;
const tps = 10;

function fetch() {
    // get page cache if needed
    const targetHighAddr = state.programCounter.get() >> specs.pageCacheSize;

    if (state.forceReadPage.get() != 0 ||
        targetHighAddr != state.rom_cache.addr.get()) {

        state.forceReadPage.set(0);
        state.rom_cache.addr.set(targetHighAddr);

        for (let i = 0; i < (1 << specs.pageCacheSize); i++) {
            const instruction = state.rom.data[(targetHighAddr << specs.pageCacheSize) + i].get();

            state.rom_cache.hi.data[i].set(instruction >> specs.instructionSize / 2);
            state.rom_cache.lo.data[i].set(instruction & ((specs.instructionSize / 2) - 1));
        }
    }

    const i = state.programCounter.get() & ((1 << specs.pageCacheSize) - 1);
    const opcode = state.rom_cache.hi.data[i].get();
    const immediate = state.rom_cache.lo.data[i].get();
    const argsHi = immediate >> (specs.instructionSize / 4);
    const argsLo = immediate & ((1 << specs.instructionSize / 4) - 1);

    return [opcode, immediate, argsHi, argsLo];
}

function mmCacheQuery(value, addr, writeToggle) {
    // TODO check hardware behavior for equal unused counters values

    const targetHighAddr = addr >> specs.mainMemoryCacheSize;
    const innerIndex = addr & ((1 << specs.mainMemoryCacheSize) - 1);

    // check for cache hit
    for (let i = 0; i < specs.mainMemoryCacheModules; i++) {
        const chosen = state.mainMemoryCache[i];

        if (targetHighAddr == cache.highAddress.get()) {
            // make the other counters unused
            for (let j = 0; j < specs.mainMemoryCacheModules; j++) {
                if (i == j) continue;
                const other = state.mainMemoryCache[j];
                other.unusedCounter.set(other.unusedCounter.get() + 1);
            }

            chosen.unusedCounter.set(0);
            if (writeToggle)
                chosen.data[innerIndex].set(value);
            else
                return chosen.data[innerIndex].get();
        }
    }

    // no cache hit, swap data with least recently used cache module
    let lru_index, lru_counter;
    for (let i = 0; i < specs.mainMemoryCacheModules; i++) {
        const module = state.mainMemoryCache[i];
        const counter = module.unusedCounter.get();
        if (i == 0 || counter > lru_counter) {
            lru_index = i;
            lru_counter = counter;
        }
    }

    // swap
    const chosen = state.mainMemoryCache[lru_index];
    for (let i = 0; i < (1 << specs.mainMemoryCacheSize); i++)
        state.mainMemory.data[(chosen.highAddress << specs.mainMemoryCacheSize) + i].set(
            chosen.data[i].get()
        );
    for (let i = 0; i < (1 << specs.mainMemoryCacheSize); i++)
        chosen.data[i].set(
            state.mainMemory.data[(targetHighAddr << specs.mainMemoryCacheSize) + i].get()
        );

    // return now cached value
    chosen.highAddress.set(targetHighAddr);
    chosen.unusedCounter.set(0);
    if (writeToggle)
        chosen.data[innerIndex].set(value);
    else
        return chosen.data[innerIndex].get();
}

function execute(opcode, immediate, argsHi, argsLo) {
    state.programCounter.set(state.programCounter.get() + 1);

    // common actions
    // reg r1 = dual(hi, lo)
    const r1Addr = {
        'A': argsHi % specs.registers,
        'B': argsLo % specs.registers
    };
    // trigger r1
    const r1 = {
        'A': state.registers.data[r1Addr.A].get(),
        'B': state.registers.data[r1Addr.B].get()
    };
    // reg r2 = dual(-, B)
    const r2Addr = {
        'A': 0, // unused
        'B': r1.B % specs.registers
    };
    // reg w = 0 (default value)
    let regWAddr = 0;
    // MM value = B
    const mmValue = r1.B;
    // MM addr = A
    const mmAddr = r1.A;
    // B to IO value
    const ioValue = r1.B;
    // A to IO addr
    const ioAddr = r1.A;
    // PC into stack value
    const stackValue = state.programCounter.get();

    let bufferedFromA, r2, mmRes;

    // instruction-specific actions
    switch(getInstructionName(opcode)) {
        case "NOP":
            break;
        case "COND":
            // cond value = SR & immediate
            state.conditionBuffer.set(state.stateRegister.get() & immediate);
            break;
        case "JUMPI":
            if (state.stateRegister.get() != 0) {
                // PC value = immediate, PC jump
                state.programCounter.set(immediate);
            }
            break;
        case "JUMP":
            if (state.stateRegister.get() != 0) {
                // PC value = A/B, PC jump
                state.programCounter.set((r1.A << specs.wordSize) + r1.B);
            }
            break;
        case "JUMPR":
            if (state.stateRegister.get() != 0) {
                // PC value = A/B, PC relative jump
                state.programCounter.set(state.programCounter.get() + (r1.A << specs.wordSize) + r1.B);
            }
            break;
        case "LDI":
            // reg trigger W, allow immediate into reg
            state.registers.data[regWAddr].set(immediate);
            break;
        case "MOVEI":
            // reg trigger W, allow B into reg
            state.registers.data[regWAddr].set(r1.B);
            break;
        case "MOVEA":
            // reg buffer A on r1
            bufferedFromA = r1.A;
            // reg trigger r2
            r2 = {
                'A': state.registers.data[r2Addr.A].get(),
                'B': state.registers.data[r2Addr.B].get()
            };
            // reg w = A (from buffer)
            regWAddr = bufferedFromA;
            // reg trigger W, allow B into reg
            state.registers.data[regWAddr].set(r2.B);
            break;
        case "RTC":
            // MM cache query, MM cache write toggle
            mmCacheQuery(mmValue, mmAddr, true);
            break;
        case "CTR":
            // MM cache query
            regWAddr = argsLo % specs.registers;
            mmRes = mmCacheQuery(mmValue, mmAddr, false);
            // reg trigger W, allow MM into reg
            state.registers.data[regWAddr].set(mmRes);
            break;
        case "TIMER":
            // reg w = hi
            regWAddr = argsHi % specs.registers;
            // reg trigger W, allow timer into reg
            state.registers.data[regWAddr].set(state.timer.get());
            break;
        default:
            console.error("Found unacceptable instruction with opcode " + opcode);
    }
}

function step() {
    const [opcode, immediate, argsHi, argsLo] = fetch();
    execute(opcode, immediate, argsHi, argsLo);

    ui.display();
    state.timer.set(state.timer.get() + 1);
}

function run() {
    if (interval == null)
        interval = self.setInterval(step, 1000/tps);
}

function stop() {
    if (interval != null)
        self.clearInterval(interval);
    interval = null;
}

function reset() {
    state.programCounter.reset();
    state.reset(true);

    ui.display();
}

function fullReset() {
    state.reset();
    reset();
}

export function setupRun(_state, _ui) {
    state = _state;
    ui = _ui;

    document.getElementById("btn-step").onclick = step;
    document.getElementById("btn-run").onclick = run;
    document.getElementById("btn-stop").onclick = stop;
    document.getElementById("btn-reset").onclick = reset;
    document.getElementById("btn-full-reset").onclick = fullReset;
}
