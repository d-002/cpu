import specs from "./specs.js";
import { getInstructionName, formatInstruction } from "./instructions.js"

class Data {
    #value;

    constructor(size, timer = null) {
        this.size = size;
        this.#value = 0;
        this.timer = timer;

        this.readTime = -1;
        this.writeTime = -1;
    }

    getSilent() {
        return this.#value;
    }

    setSilent(value) {
        this.#value = value & ((1 << this.size) - 1);
    }

    get() {
        if (this.timer != null)
            this.readTime = timer.#value;

        return this.#value;
    }

    set(value) {
        if (this.timer != null)
            this.writeTime = timer.#value;

        this.setSilent(value);
    }

    display(show = {}) {
        let simple;
        const detailed = [];
        const timings = [];

        if (show.asOpcode)
            simple = getInstructionName(this.#value);
        else if (show.asInstruction)
            simple = formatInstruction(this.#value);
        else {
            simple = this.#value.toString(16);
            if (simple.length == 1)
                simple = "0" + simple;
        }

        // split this.#value into 4-bit parts for ease of reading
        const parts = [];
        let value = this.#value;
        if (value) {
            parts.unshift(value & 0xf);
            value >>= 4;
        }
        else
            parts.push(0);

        detailed.push("dec: " + this.#value.toString(10));
        detailed.push("bin: " + parts.join(" "));
        detailed.push("hex: " + this.#value.toString(16));

        timings.push(this.readTime == -1
            ? "Never read"
            : "Last read at " + this.readTime + " (-" + (this.timer.#value - this.readTime) + ")");
        timings.push(this.writeTime == -1
            ? "Never written"
            : "Last write at " + this.writeTime + " (-" + (this.timer.#value - this.writeTime) + ")");

        return {
            simple: simple,
            detailed: detailed,
            timings: timings,
        };
    }
}

class DataArray {
    #data;

    constructor(nmemb, size, timer, show={}) {
        this.#data = [];
        this.nmemb = nmemb;
        this.size = size;

        for (let i = 0; i < nmemb; i++)
            this.#data.push(new Data(size, timer, show));
    }

    getSilent(i) {
        return this.#data[i];
    }

    setSilent(i, value) {
        this.#data[i] = value & ((1 << this.size) - 1);
    }
}

class State {
    constructor() {
        this.timer = new Data(specs.timerSize, null);

        this.rom = new DataArray(specs.romLength, specs.instructionSize, this.timer);
        this.rom_cache = {
            hi: new DataArray(specs.romCacheLength, specs.instructionSize / 2, this.timer),
            lo: new DataArray(specs.romCacheLength, specs.instructionSize / 2, this.timer),
        };

        this.registers = new DataArray(specs.registers, specs.wordSize, this.timer);
        this.mainMemory = new DataArray(specs.mainMemory, specs.wordSize, this.timer);
        this.mainMemoryCache = [];
        this.io = new DataArray(specs.io, specs.wordSize, this.timer);
        this.stack = new DataArray(specs.stack, specs.instructionSize, this.timer);

        for (let i = 0; i < specs.mainMemoryCacheModules; i++)
            this.mainMemoryCache.push({
                highAddress: new Data(specs.wordSize / 2, this.timer),
                unusedCounter: new Data(specs.wordSize / 2, this.timer),
                data: new DataArray(specs.mainMemoryCache, specs.wordSize, this.timer),
            });

        this.stackIndex = 0;
        this.aluBuffer = new Data(specs.wordSize, this.timer);
        this.stateRegister = new Data(specs.wordSize, this.timer);
        this.conditionBuffer = new Data(specs.wordSize, this.timer);
        this.programCounter = new Data(specs.wordSize, this.timer);
    }
}

export default State;
