const specs = {
    wordSize: 8,

    romLength: 512,
    instructionSize: 16,
    numInstructions: 32,

    pageCacheSize: 3,

    registers: 8,

    mainMemory: 256,

    mainMemoryCacheSize: 3,
    mainMemoryCacheModules: 2,

    io: 10,

    stack: 12,

    timerSize: 16, // actually 32 but Number(BigInt) can freeze the browser
};

export default specs;
