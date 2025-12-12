import specs from "./specs.js";

let state, ui;

function rom2file() {
    const exported = [];
    state.rom.data.forEach(data => exported.push(data.getSilent()));
    console.log(JSON.stringify(exported));
}

function file2rom() {
    let ok = true;

    let data;
    try {
        data = JSON.parse("");
    }
    catch (_) {
        ok = false;
    }

    if (!ok || data == null || data.length != specs.romLength) {
        console.log("Failed to import");
        return;
    }

    for (let i = 0; i < specs.romLength; i++)
        state.rom.data[i].setSilent(parseInt(data[i]) || 0);

    ui.display();
}

export function setupImportExport(_state, _ui) {
    state = _state;
    ui = _ui;

    document.getElementById("import").onclick = file2rom;
    document.getElementById("export").onclick = rom2file;
}
