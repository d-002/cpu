let state, ui;

let interval;
const tps = 10;

function fetch() {
    // get page cache if needed
    

    const opcode = value >> specs.instructionSize / 2;
    const args = value & ((1 << specs.instructionSize / 2) - 1);
    const argsHi = args >> (specs.instructionSize / 4);
    const argsLo = args & ((1 << (specs.instructionSize / 4)) - 1);

    return [opcode, args, argsHi, argsLo];
}

function execute(opcode, args, argsHi, argsLo) {
}

function step() {
    ui.display();

    const [opcode, args, argsHi, argsLo] = fetch();
    execute(opcode, args, argsHi, argsLo);
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

    document.getElementById("step").onclick = step;
    document.getElementById("run").onclick = run;
    document.getElementById("stop").onclick = stop;
    document.getElementById("reset").onclick = reset;
    document.getElementById("full-reset").onclick = fullReset;
}
