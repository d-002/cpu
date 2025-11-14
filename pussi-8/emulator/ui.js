import specs from "./specs.js";

class UiManager {
    constructor(state) {
        this.state = state;

        const get_list = id => document.getElementById(id).querySelector(".list");
        const get_span = id => document.getElementById(id).querySelector(".lone-data");

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

    displayList(elt, arr, detailed = false) {
        elt.innerHTML = "";

        let line;

        for (let i = 0; i < arr.nmemb; i++) {
            if (i % 8 == 0) {
                if (line)
                    elt.appendChild(line);
                line = document.createElement("div");
            }

            const display = arr.getSilent(i).display();

            const div = document.createElement("div");
            div.className = "data";
            if (detailed) {
                const pre = document.createElement("span")
                const editable = document.createElement("input");
                const post = document.createElement("span");
                pre.textContent = i + " :";
                editable.setAttribute("type", "number");
                editable.setAttribute("min", "0");
                editable.value = display.simple;
                post.textContent = display.detailed.join("\n");
                div.appendChild(pre);
                div.appendChild(editable);
                div.appendChild(post);
            }
            else {
                const editable = document.createElement("input");
                const onHover = document.createElement("span");
                editable.setAttribute("type", "number");
                editable.setAttribute("min", "0");
                editable.value = display.simple;
                onHover.innerHTML = display.detailed.join("<br>");
                onHover.class = "on-hover";
                div.appendChild(editable);
                div.appendChild(onHover);
            }

            line.appendChild(div);
        }

        if (line)
            elt.appendChild(line);
    }

    displayData(elt, data, show = {}) {
        elt.innerHTML = data.display(show).simple;
    }

    display() {
        this.displayList(this.elts.romPage.hi, this.state.rom_cache.hi, true);
        this.displayList(this.elts.romPage.lo, this.state.rom_cache.lo, true);
        this.displayData(this.elts.romPage.current,
            this.state.rom.getSilent(this.state.programCounter.getSilent()),
            { asInstruction: true });

        this.displayList(this.elts.rom, this.state.rom);

        this.displayList(this.elts.registers, this.state.registers, true);

        for (let i = 0; i < specs.mainMemoryCacheModules; i++) {
            const elt = this.elts.memoryCache[i];
            const data = this.state.mainMemoryCache[i];
            this.displayList(elt.list, data.data, true);
            this.displayData(elt.highAddress, data.highAddress);
        }

        this.displayList(this.elts.mainMemory, this.state.mainMemory);

        this.displayList(this.elts.io, this.state.io, true);

        this.displayList(this.elts.stack.list, this.state.stack, true);
        this.elts.stack.index.textContent = this.state.stackIndex;

        this.displayData(this.elts.buffers.alu, this.state.aluBuffer);
        this.displayData(this.elts.buffers.stateRegister, this.state.stateRegister);
        this.displayData(this.elts.buffers.conditionBuffer, this.state.conditionBuffer);
        this.displayData(this.elts.buffers.programCounter, this.state.programCounter);
        this.displayData(this.elts.buffers.timer, this.state.timer);
    }
}

export default UiManager;
