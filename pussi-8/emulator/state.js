import specs from "./specs.js";
import { getInstructionName, formatInstruction } from "./instructions.js"

export class Data {
    #value;

    constructor(size, timer = null) {
        this.size = size;
        this.timer = timer;

        this.reset();
    }

    reset(onlyTimes) {
        if (!onlyTimes)
            this.#value = 0;
        this.readTime = -1;
        this.writeTime = -1;
    }

    getSilent() {
        return this.#value;
    }

    setSilent(value) {
        const mask = (BigInt(1) << BigInt(this.size)) - BigInt(1);
        this.#value = Number(BigInt(value) & mask);
    }

    get() {
        if (this.timer != null)
            this.readTime = this.timer.#value;

        return this.#value;
    }

    set(value) {
        if (this.timer != null)
            this.writeTime = this.timer.#value;

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
        else if (this.size == 1)
            simple = this.#value == 0 ? "false" : "true";
        else
            simple = this.#value.toString(16).toUpperCase().padStart(this.size / 4, "0");

        // split this.#value into 4-bit parts for ease of reading
        const parts = [];
        let value = this.#value;
        let count = 0;
        while (count++ < this.size / 4 || value) {
            parts.unshift((value & 0xf).toString().padStart(4, "0"));
            value >>= 4;
        }

        const num_dec_digits = Math.ceil(this.size * Math.log10(2));
        detailed.push("dec: " + this.#value.toString().padStart(num_dec_digits, "0"));
        detailed.push("bin: " + parts.join("_"));
        detailed.push("hex: 0x" + this.#value.toString(16).toUpperCase().padStart(this.size / 4, "0"));

        timings.push(this.readTime == -1
            ? "Never read"
            : "Last read at " + this.readTime + " (" + (this.timer.#value - this.readTime) + " ago)");
        timings.push(this.writeTime == -1
            ? "Never written"
            : "Last write at " + this.writeTime + " (" + (this.timer.#value - this.writeTime) + " ago)");

        return {
            simple: simple,
            detailed: detailed,
            timings: timings,
        };
    }
}

class DataArray {
    constructor(nmemb, size, timer, show={}) {
        this.data = [];
        this.nmemb = nmemb;
        this.size = size;

        for (let i = 0; i < nmemb; i++)
            this.data.push(new Data(size, timer, show));
    }

    reset(onlyTimes) {
        this.data.forEach(data => data.reset(onlyTimes));
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

        this.stackIndex = new Data(specs.wordSize, this.timer);
        this.aluBuffer = new Data(specs.wordSize, this.timer);
        this.stateRegister = new Data(specs.wordSize, this.timer);
        this.conditionBuffer = new Data(1, this.timer);
        this.forceReadPage = new Data(1, this.timer);
        this.programCounter = new Data(specs.wordSize, this.timer);

        this.reset();
    }

    reset(onlyTimes = false) {
        this.timer.reset(onlyTimes);
        this.rom.reset(onlyTimes);
        this.rom_cache.hi.reset(onlyTimes);
        this.rom_cache.lo.reset(onlyTimes);
        this.registers.reset(onlyTimes);
        this.mainMemory.reset(onlyTimes);
        this.mainMemoryCache.forEach(cache => {
            cache.highAddress.reset(onlyTimes);
            cache.unusedCounter.reset(onlyTimes);
            cache.data.reset(onlyTimes);
        });
        this.stackIndex.reset(onlyTimes);
        this.aluBuffer.reset(onlyTimes);
        this.stateRegister.reset(onlyTimes);
        this.stateRegister.setSilent(2);
        this.stateRegister.reset(true);
        this.conditionBuffer.reset(onlyTimes);
        this.forceReadPage.setSilent(1);
        this.forceReadPage.reset(true);
        this.programCounter.reset(onlyTimes);
    }

    updateState(path, value) {
        if (value == "")
            value = "0";

        value = BigInt("0x" + value);

        let target_var = null;

        while (true) {
            const i = path.indexOf(".");
            if (i == -1) {
                if (target_var == null)
                    target_var = this[path];
                else
                    target_var = target_var[path];
                break;
            }

            if (target_var == null)
                target_var = this[path.substring(0, i)];
            else
                target_var = target_var[path.substring(0, i)];

            path = path.substring(i + 1);
        }

        target_var.set(value);
    }
}

export default State;
