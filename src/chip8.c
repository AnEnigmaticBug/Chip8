#include "chip8.h"

#define ROM_START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50
#define FONTSET_SIZE 80

#define xreg_idx(opcode) ((opcode & 0x0F00u) >> 8u)
#define yreg_idx(opcode) ((opcode & 0x00F0u) >> 4u)

uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

void chip8_init(Chip8 *self)
{
    self->pc = ROM_START_ADDRESS;

    for (int i = 0; i < FONTSET_SIZE; ++i)
    {
        self->memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

void chip8_load_rom(Chip8 *self, FILE *rom_fp)
{
    fseek(rom_fp, 0, SEEK_END);
    size_t file_size = ftell(rom_fp);
    fseek(rom_fp, 0, SEEK_SET);

    fread(self->memory + ROM_START_ADDRESS, sizeof(uint8_t), file_size, rom_fp);
}

static void op_00E0(Chip8 *self)
{
    memset(self->video_memory, 0, sizeof(self->video_memory));
}

static void op_00EE(Chip8 *self)
{
    self->pc = self->stack[--(self->sp)];
}

static void op_1nnn(Chip8 *self)
{
    self->pc = self->opcode & 0x0FFFu;
}

static void op_2nnn(Chip8 *self)
{
    uint16_t address = self->opcode & 0x0FFFu;

    self->stack[(self->sp)++] = self->pc;
    self->pc = address;
}

static void op_3xkk(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t byte = self->opcode & 0x00FFu;

    if (self->registers[ix] == byte)
    {
        self->pc += 2;
    }
}

static void op_4xkk(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t byte = self->opcode & 0x00FFu;

    if (self->registers[ix] != byte)
    {
        self->pc += 2;
    }
}

static void op_5xy0(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t iy = yreg_idx(self->opcode);

    if (self->registers[ix] == self->registers[iy])
    {
        self->pc += 2;
    }
}

static void op_6xkk(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t byte = self->opcode & 0x00FFu;

    self->registers[ix] = byte;
}

static void op_7xkk(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t byte = self->opcode & 0x00FFu;

    self->registers[ix] += byte;
}

static void op_8xy0(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t iy = yreg_idx(self->opcode);

    self->registers[ix] = self->registers[iy];
}

static void op_8xy1(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t iy = yreg_idx(self->opcode);

    self->registers[ix] |= self->registers[iy];
}

static void op_8xy2(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t iy = yreg_idx(self->opcode);

    self->registers[ix] &= self->registers[iy];
}

static void op_8xy3(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t iy = yreg_idx(self->opcode);

    self->registers[ix] ^= self->registers[iy];
}

static void op_8xy4(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t iy = yreg_idx(self->opcode);

    uint16_t sum = self->registers[ix] + self->registers[iy];

    self->registers[15] = sum > 255u;
    self->registers[ix] = sum & 0x00FFu;
}

static void op_8xy5(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t iy = yreg_idx(self->opcode);

    self->registers[15] = self->registers[ix] > self->registers[iy] ? 1 : 0;
    self->registers[ix] -= self->registers[iy];
}

static void op_8xy6(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    self->registers[15] = self->registers[ix] & 0x01u;
    self->registers[ix] >>= 1;
}

static void op_8xy7(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t iy = yreg_idx(self->opcode);

    self->registers[15] = self->registers[iy] > self->registers[ix] ? 1 : 0;
    self->registers[ix] = self->registers[iy] - self->registers[iy];
}

static void op_8xyE(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    self->registers[15] = (self->registers[ix] & 0x80u) >> 7u;
    self->registers[ix] <<= 1;
}

static void op_9xy0(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t iy = yreg_idx(self->opcode);

    if (self->registers[ix] != self->registers[iy])
    {
        self->pc += 2;
    }
}
