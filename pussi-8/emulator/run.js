import specs from "./specs.js";

let state, ui;

let interval;
const tps = 10;

function fetch() {
    // get page cache if needed
    const targetCacheAddr = state.programCounter.get() >> specs.pageCacheSize;

    if (state.forceReadPage.get() != 0 ||
        targetCacheAddr != state.rom_cache.addr.get()) {

        state.forceReadPage.set(0);
        state.rom_cache.addr.set(targetCacheAddr);

        for (let i = 0; i < (1 << specs.pageCacheSize); i++) {
            const instruction = state.rom.data[(targetCacheAddr << specs.pageCacheSize) + i].get();

            state.rom_cache.hi.data[i].set(instruction >> specs.instructionSize / 2);
            state.rom_cache.lo.data[i].set(instruction & ((specs.instructionSize / 2) - 1));
        }
    }

    const i = state.programCounter.get() & ((1 << specs.pageCacheSize) - 1);
    const opcode = state.rom_cache.hi.data[i];
    const args = state.rom_cache.lo.data[i];
    const argsHi = args >> (specs.instructionSize / 4);
    const argsLo = args & ((1 << specs.instructionSize / 4) - 1);

    return [opcode, args, argsHi, argsLo];
}

function execute(opcode, args, argsHi, argsLo) {
    state.programCounter.set(state.programCounter.get() + 1);
}

function step() {
    const [opcode, args, argsHi, argsLo] = fetch();
    execute(opcode, args, argsHi, argsLo);

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
