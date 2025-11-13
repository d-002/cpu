import specs from "./specs.js";

class UiManager {
    constructor(state) {
        this.state = state;

        const get_list = id => document.getElementById(id).querySelector(".list");
        const get_span = id => document.getElementById(id).querySelector("span");

        this.elts = {
            romPage: {
                hi: get_list("rom-page-hi"),
                lo: get_list("rom-page-lo"),
                current: get_span("rom-page"),
            },
            rom: get_list("rom-memory"),
            registers: get_list("registers"),
            memoryCache: [],
            mainMemory: get_list("main-memory"),
            io: get_list("io"),
            stack: {
                list: get_list("stack"),
                index: get_span("stack"),
            },
            buffers: {},
        };

        Array.from(document.querySelectorAll("#memory-cache>.block")).forEach(elt => {
                this.elts.memoryCache.push({
                    list: elt.querySelector(".list"),
                    highAddress: elt.querySelector("span"),
                });
        });
        const spanElements = document.querySelectorAll("#buffers>span");
        this.elts.buffers.alu = spanElements[0];
        this.elts.buffers.stateRegister = spanElements[1];
        this.elts.buffers.conditionBuffer = spanElements[2];
        this.elts.buffers.programCounter = spanElements[3];
        this.elts.buffers.timer = spanElements[4];
    }

    displayList(elt, arr) {
        const lines = [];

        for (let i = 0; i < arr.nmemb; i++) {
            if (i % 8 == 0)
                lines.push([]);
            lines[lines.length - 1].push(arr.getSilent(i).display().simple);
        }

        for (let i = 0; i < lines.length; i++)
            lines[i] = lines[i].join(" ");

        elt.innerHTML = lines.join("<br>");
    }

    displayListDetailed(elt, arr) {
        const lines = [];

        for (let i = 0; i < arr.nmemb; i++)
            lines.push(i + ": " + arr.getSilent(i).display().detailed.join(" "));

        elt.innerHTML = lines.join("<br>");
    }

    displayData(elt, data, show = {}) {
        elt.innerHTML = data.display(show).simple;
    }

    display() {
        this.displayListDetailed(this.elts.romPage.hi, this.state.rom_cache.hi);
        this.displayListDetailed(this.elts.romPage.lo, this.state.rom_cache.lo);
        this.displayData(this.elts.romPage.current,
            this.state.rom.getSilent(this.state.programCounter.getSilent()),
            { asInstruction: true });

        this.displayList(this.elts.rom, this.state.rom);

        this.displayListDetailed(this.elts.registers, this.state.registers);

        for (let i = 0; i < specs.mainMemoryCacheModules; i++) {
            const elt = this.elts.memoryCache[i];
            const data = this.state.mainMemoryCache[i];
            this.displayListDetailed(elt.list, data.data);
            this.displayData(elt.highAddress, data.highAddress);
        }

        this.displayList(this.elts.mainMemory, this.state.mainMemory);

        this.displayListDetailed(this.elts.io, this.state.io);

        this.displayListDetailed(this.elts.stack.list, this.state.stack);
        this.elts.stack.index.textContent = this.state.stackIndex;

        this.displayData(this.elts.buffers.alu, this.state.aluBuffer);
        this.displayData(this.elts.buffers.stateRegister, this.state.stateRegister);
        this.displayData(this.elts.buffers.conditionBuffer, this.state.conditionBuffer);
        this.displayData(this.elts.buffers.programCounter, this.state.programCounter);
        this.displayData(this.elts.buffers.timer, this.state.timer);
    }
}

export default UiManager;
