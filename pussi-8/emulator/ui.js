import { Data } from "./state.js";
import specs from "./specs.js";

class UiManager {
    constructor(state) {
        this.state = state;

        const get_list = id => document.getElementById(id).querySelector("div.list");
        const get_span = (id, i = 0) => document.getElementById(id).querySelectorAll("span.lone-data")[i];

        this.elts = {
            romPage: {
                hi: get_list("rom-page-hi"),
                lo: get_list("rom-page-lo"),
                addr: get_span("rom-page", 0),
                current: get_span("rom-page", 1),
            },
            rom: get_list("rom-memory"),
            registers: get_list("registers"),
            memoryCache: [],
            mainMemory: get_list("main-memory"),
            io: get_list("io"),
            stack: {
                list: get_list("stack"),
                index: get_span("stack", 0),
            },
            buffers: {},
            stateRegister: {
                compact: get_span("state-register"),
                list: document.querySelectorAll("#state-register>div.lone-data"),
            },
        };

        Array.from(document.querySelectorAll("#memory-cache>.block")).forEach(elt => {
            const [unusedCounter, highAddress] = elt.querySelectorAll(".lone-data");
            this.elts.memoryCache.push({
                list: elt.querySelector(".list"),
                unusedCounter: unusedCounter,
                highAddress: highAddress,
            });
        });
        const spanElements = document.querySelectorAll("#buffers>span.lone-data");
        this.elts.buffers.alu = spanElements[0];
        this.elts.buffers.conditionBuffer = spanElements[1];
        this.elts.buffers.forceReadPage = spanElements[2];
        this.elts.buffers.programCounter = spanElements[3];
        this.elts.buffers.timer = spanElements[4];
    }

    displayList(elt, arr, path, format_i, detailed) {
        elt.innerHTML = "";

        let line;

        for (let i = 0; i < arr.nmemb; i++) {
            if (i % 8 == 0 || detailed) {
                if (line)
                    elt.appendChild(line);
                line = document.createElement("div");

                if (!detailed) {
                    const pre = document.createElement("span")
                    pre.textContent = format_i(i / 8) + " :";
                    line.appendChild(pre);
                }
            }

            const data = arr.data[i];
            const display = data.display();

            const div = document.createElement("div");
            const editable = document.createElement("input");
            const onHover = document.createElement("span");

            div.className = "data";
            editable.setAttribute("type", "text");
            editable.setAttribute("min", "0");
            editable.value = display.simple;
            onHover.className = "on-hover";

            if (detailed) {
                const pre = document.createElement("span")
                const post = document.createElement("span");
                pre.textContent = format_i(i) + " :";
                onHover.innerHTML = display.timings.join("<br>");
                post.textContent = "| " + display.detailed.join(", ");
                div.appendChild(pre);
                div.appendChild(editable);
                div.appendChild(post);
            }
            else {
                onHover.innerHTML = [...display.detailed, ...display.timings].join("<br>");
                div.appendChild(editable);
            }

            div.appendChild(onHover);
            div.setAttribute("path", path + ".data." + i);
            this.colorData(div, data);
            line.appendChild(div);
        }

        if (line)
            elt.appendChild(line);
    }

    displayData(elt, data, path, raw, show = {}) {
        elt.innerHTML = "";

        const display = data.display(show);

        const div = document.createElement("div");
        const center = document.createElement(raw ? "span" : "input");
        const onHover = document.createElement("span");

        div.className = "data";
        if (raw)
            center.textContent = display.simple;
        else {
            center.setAttribute("type", "text");
            center.setAttribute("min", "0");
            center.value = display.simple;
        }
        onHover.innerHTML = [...display.detailed, ...display.timings].join("<br>");
        onHover.className = "on-hover";
        div.appendChild(center);
        div.appendChild(onHover);
        div.setAttribute("path", path);
        this.colorData(div, data);

        elt.appendChild(div);
    }

    colorData(elt, data) {
        if (data.timer == null)
            return;

        const now = data.timer.getSilent();
        const readDelay = data.readTime == -1 ? 1000 : now - data.readTime;
        const writeDelay = data.writeTime == -1 ? 1000 : now - data.writeTime;

        if (readDelay < 3)
            elt.classList.add("readDist" + readDelay);
        else if (writeDelay < 3)
            elt.classList.add("writeDist" + writeDelay);
    }

    display() {
        const show_i = i => i;
        const show_i_mul = i => (8 * i).toString(16).toUpperCase().padStart(4, "0");
        const show_stack = i => i == 0 ? "btm" : i == specs.stack - 1 ? "top" : i;

        this.displayList(this.elts.romPage.hi, this.state.rom_cache.hi, "rom_cache.hi", show_i, true);
        this.displayList(this.elts.romPage.lo, this.state.rom_cache.lo, "rom_cache.lo", show_i, true);

        const rom_cache_index = this.state.programCounter.getSilent() % (1 << specs.romCacheSize);
        const opcode = this.state.rom_cache.hi.data[rom_cache_index];
        const args = this.state.rom_cache.lo.data[rom_cache_index];
        const temp_data_element = new Data(specs.instructionSize, this.state.timer);
        temp_data_element.setSilent((opcode.getSilent() << specs.wordSize) + args.getSilent());
        this.displayData(this.elts.romPage.addr, temp_data_element, null, false);
        this.displayData(this.elts.romPage.current,
            temp_data_element,
            null,
            true,
            { asInstruction: true });

        this.displayList(this.elts.rom, this.state.rom, "rom", show_i_mul, false);

        this.displayList(this.elts.registers, this.state.registers, "registers", show_i, true);

        for (let i = 0; i < specs.mainMemoryCacheModules; i++) {
            const elt = this.elts.memoryCache[i];
            const data = this.state.mainMemoryCache[i];
            const path_prefix = "mainMemoryCache." + i + ".";
            this.displayList(elt.list, data.data, path_prefix + "data", show_i, true);
            this.displayData(elt.unusedCounter, data.unusedCounter, path_prefix + "unusedCounter", false);
            this.displayData(elt.highAddress, data.highAddress, path_prefix + "highAddress", false);
        }

        this.displayList(this.elts.mainMemory, this.state.mainMemory, "mainMemory", show_i_mul, false);

        this.displayList(this.elts.io, this.state.io, "io", show_i, true);

        this.displayList(this.elts.stack.list, this.state.stack, "stack", show_stack, true);
        this.displayData(this.elts.stack.index, this.state.stackIndex, "stackIndex", false);

        this.displayData(this.elts.buffers.alu, this.state.aluBuffer, "aluBuffer", false);
        this.displayData(this.elts.buffers.conditionBuffer, this.state.conditionBuffer, "conditionBuffer", false);
        this.displayData(this.elts.buffers.forceReadPage, this.state.forceReadPage, "forceReadPage", false);
        this.displayData(this.elts.buffers.programCounter, this.state.programCounter, "programCounter", false);
        this.displayData(this.elts.buffers.timer, this.state.timer, "timer", false);

        this.displayData(this.elts.stateRegister.compact, this.state.stateRegister, "stateRegister", false);
        const names = [
            F => "1: 0 - constant zero (" + F + ")",
            T => "2: 1 - constant one (" + T + ")",
            U => "4: U - stack underflow (" + U + ")",
            O => "8: O - stack overflow (" + O + ")",
            V => "16: V - ALU overflow (" + V + ")",
            N => "32: N - ALU negative (" + N + ")",
            C => "64: C - ALU carry (" + C + ")",
            Z => "128: Z - ALU zero (" + Z + ")",
        ];
        for (let i = 0; i < 8; i++) {
            const value = this.state.stateRegister.getSilent() & (1 << i);
            const text = names[i](value ? 1 : 0);
            this.elts.stateRegister.list[7 - i].textContent = text;
        }
    }
}

export default UiManager;
