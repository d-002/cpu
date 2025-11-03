from settings import *

class Data:
    def __init__(self):
        self.rom = [0] * ROM
        self.rom_cache = {
                'opcode': [0] * ROM_CACHE,
                'args': [0] * ROM_CACHE }
        self.registers = [0] * REGISTERS
        self.ram = [0] * RAM
        self.ram_cache = [
                [0] * RAM_CACHE
                for _ in range(RAM_CACHE_MODULES)]

        self.stack = [0] * STACK
        self.stack_index = -1

        self.io_ports = [0] * IO_PORTS

        self.alu_buf = 0
        self.sr = 0b00000010
        self.program_counter = 0
