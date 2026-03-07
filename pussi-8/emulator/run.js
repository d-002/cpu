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

            state.rom_cache.hi.data[i].set(instruction >> (specs.instructionSize >> 1));
            state.rom_cache.lo.data[i].set(instruction & ((1 << (specs.instructionSize >> 1)) - 1));
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
        if (lru_index == null || counter > lru_counter) {
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

function setFlagTo(flagName, value) {
    let bit;
    switch(flagName) {
        case "Z": bit = 128; break;
        case "C": bit = 64; break;
        case "N": bit = 32; break;
        case "V": bit = 16; break;
        case "O": bit = 8; break;
        case "U": bit = 4; break;
    }

    if (bit == null) {
        console.log("Trying to edit the state register with an invalid flag name: " + flagName);
        return;
    }

    if (value)
        state.stateRegister.set(state.stateRegister.get() | bit);
    else
        state.stateRegister.set(state.stateRegister.get() & ((1 << specs.wordSize) - 1 - bit));
}

function getAluRes(A, B, instructionName) {
    // failsafe to avoid propagating null values
    let res = 0;

    // compute result
    const flip = value => (1 << specs.wordSize) - 1 - value;
    switch(instructionName) {
        case "OR": res = A | B; break;
        case "NOR": res = flip(A | B); break;
        case "ADD": res = A + B; break;
        case "SUB": res = A < B ? flip(B - A) : A - B; break;
        case "XOR": res = A ^ B; break;
        case "XNOR": res = flip(A ^ B); break;
        case "AND": res = A & B; break;
        case "NAND": res = flip(A & B); break;
        case "NOT": res = flip(A); break;
        case "LSH": res = A << 1; break;
        case "RSH": res = A >> 1; break;
        case "ROR": res = (A >> 1) + ((A & 1) << specs.wordSize - 1); break;
        case "ROL": res = (A << 1) + (A >> specs.wordSize - 1); break;
    }

    // edit flags
    const signA = (A >> (specs.wordSize - 1)) & 1;
    const signB = (B >> (specs.wordSize - 1)) & 1;
    const signRes = (res >> (specs.wordSize - 1)) & 1;
    setFlagTo("Z", res == 0);
    setFlagTo("C", res >> specs.wordSize != 0);
    setFlagTo("N", signRes);
    setFlagTo("V", (signA == signB) && (signA ^ signRes));

    return res & (1 << specs.wordSize);
}

function readR1(r1Addr, a = true, b = true) {
    if (a)
        state.registers.data[r1Addr.A].get();
    if (b)
        state.registers.data[r1Addr.B].get();
}

function execute(opcode, immediate, argsHi, argsLo) {
    // common actions
    // reg r1 = dual(hi, lo)
    const r1Addr = {
        A: argsHi % specs.registers,
        B: argsLo % specs.registers
    };
    // trigger r1
    const r1 = {
        // don't trigger reads here
        A: state.registers.data[r1Addr.A].getSilent(),
        B: state.registers.data[r1Addr.B].getSilent()
    };
    // reg r2 = dual(-, B)
    const r2Addr = {
        A: 0, // unused
        B: r1.B % specs.registers
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
    const ioAddr = r1.A % specs.io;
    // PC into stack value
    const stackValue = state.programCounter.get();

    let cond, bufferedFromA, r2, mmRes;

    // instruction-specific actions
    const instructionName = getInstructionName(opcode);
    switch(instructionName) {
        case "NOP":
            break;
        case "COND":
            // cond value = SR & immediate
            cond = state.stateRegister.get() & immediate;
            state.conditionBuffer.set((immediate & 1) ? !cond : !!cond);
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
                readR1(r1Addr);
                state.programCounter.set((r1.A << specs.wordSize) + r1.B);
            }
            break;
        case "JUMPR":
            if (state.stateRegister.get() != 0) {
                // PC value = A/B, PC relative jump
                readR1(r1Addr);
                state.programCounter.set(state.programCounter.get() + (r1.A << specs.wordSize) + r1.B);
            }
            break;
        case "LDI":
            // reg trigger W, allow immediate into reg
            state.registers.data[regWAddr].set(immediate);
            break;
        case "MOVEI":
            // reg trigger W, allow B into reg
            readR1(r1Addr, false, true);
            // reg W = hi
            regWAddr = r1Addr.A;
            state.registers.data[regWAddr % specs.registers].set(r1.B);
            break;
        case "MOVEA":
            readR1(r1Addr);
            // reg buffer A on r1
            bufferedFromA = r1.A;
            // reg trigger r2
            r2 = {
                A: state.registers.data[r2Addr.A].get(),
                B: state.registers.data[r2Addr.B].get()
            };
            // reg w = A (from buffer)
            regWAddr = bufferedFromA;
            // reg trigger W, allow B into reg
            state.registers.data[regWAddr % specs.registers].set(r2.B);
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
            state.registers.data[regWAddr % specs.registers].set(mmRes);
            break;
        case "TIMER":
            // reg w = hi
            regWAddr = argsHi % specs.registers;
            // reg trigger W, allow timer into reg
            state.registers.data[regWAddr % specs.registers].set(state.timer.get());
            break;
        case "MUL":
            // reg trigger W, allow mul into reg
            readR1(r1Addr);
            state.registers.data[0].set(r1.A * r1.B);
            break;
        case "DIV":
            // reg trigger W, allow div into reg
            readR1(r1Addr);
            state.registers.data[0].set(r1.B == 0
                ? (1 << specs.wordSize) - 1
                : Math.floor(r1.A / r1.B));
            break;
        case "MOD":
            // reg trigger W, allow mod into reg
            readR1(r1Addr);
            state.registers.data[0].set(r1.B == 0 ? 0 : r1.A % r1.B);
            break;
        case "IN":
            // reg buffer A on r1
            bufferedFromA = r1.A;
            // reg w = A (from buffer)
            regWAddr = bufferedFromA;
            // reg trigger W, allow io into reg
            state.registers.data[regWAddr % specs.registers].set(state.io.data[ioAddr].get());
            break;
        case "OUT":
            // io write
            state.io.data[ioAddr].set(ioValue);
            break;
        case "OR":
        case "NOR":
        case "ADD":
        case "SUB":
        case "XOR":
        case "XNOR":
        case "AND":
        case "NAND":
        case "NOT":
        case "LSH":
        case "RSH":
        case "ROR":
        case "ROL":
            // reg trigger W, allow alu into reg
            readR1(r1Addr);
            state.registers.data[0].set(getAluRes(r1.A, r1.B, instructionName));
            break;
        case "PUSH":
            // stack push if possible
            if (state.stackIndex.get() >= specs.stack)
                setFlagTo("O", 1);
            else {
                setFlagTo("O", 0);
                state.stack.data[state.stackIndex.get()].set(stackValue);
                state.stackIndex.set(state.stackIndex.get() + 1);
            }
            break;
        case "POP":
            // stack pop if possible, pc jump
            if (state.stackIndex.get() == 0)
                setFlagTo("U", 1);
            else {
                setFlagTo("U", 0);
                state.programCounter.set(state.stack.data[state.stackIndex.get()]);
                state.stackIndex.set(state.stackIndex.get() - 1);
            }
            break;
        case "HLT":
            // stop clock if running
            stop();
            break;
        default:
            console.error("Found unacceptable instruction with opcode " + opcode);
    }
}

function step() {
    const [opcode, immediate, argsHi, argsLo] = fetch();
    execute(opcode, immediate, argsHi, argsLo);

    ui.display();

    // increment timers now to avoid messing up the display
    state.programCounter.set(state.programCounter.get() + 1);
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

function importFile() {
    const input = document.createElement("INPUT");
    input.type = "file";
    input.style = "width: 0; height: 0;"
    document.body.appendChild(input);
    input.click();
    input.addEventListener("change", () => {
        let count = 0;
        const targetCount = input.files.length;

        if (targetCount != 1)
            return;

        const file = input.files[0];
        const read = new FileReader();

        read.onloadend = () => {
            const arr = new Uint8Array(read.result);

            const length = Math.floor(arr.length / specs.instructionSize * 8);
            if (length > specs.romLength)
                alert("File too long, will be trimming it to " + specs.romLength
                    + " instructions (found " + length + ")");

            fullReset();
            for (let i = 0; i < specs.romLength; i++) {
                const opcode = 2 * i < arr.length ? arr[2 * i] : 0;
                const args = 2 * i + 1 < arr.length ? arr[2 * i + 1] : 0;
                state.rom.data[i].setSilent((opcode << (specs.instructionSize >> 1)) + args);
            }
            ui.display();

            console.log("Imported file " + file.name);
        };
        read.readAsArrayBuffer(file);

        input.remove();
    });
}

export function setupRun(_state, _ui) {
    state = _state;
    ui = _ui;

    document.getElementById("btn-step").onclick = step;
    document.getElementById("btn-run").onclick = run;
    document.getElementById("btn-stop").onclick = stop;
    document.getElementById("btn-reset").onclick = reset;
    document.getElementById("btn-full-reset").onclick = fullReset;
    document.getElementById("btn-import").onclick = importFile;

    document.addEventListener("keydown", evt => {
        if (evt.key == "r")
            run();
        else if (evt.key == "s")
            stop();
        else if (evt.key == "R")
            reset();
        else if (evt.key == "t")
            step();
        else if (evt.key == "F")
            fullReset();
        else if (evt.key == "i")
            importFile();
    });
}
