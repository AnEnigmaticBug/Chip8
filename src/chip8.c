#include "chip8.h"
#include <stdlib.h>

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

static void op_Annn(Chip8 *self)
{
    uint16_t address = self->opcode & 0x0FFFu;
    self->idx_ptr = address;
}

static void op_Bnnn(Chip8 *self)
{
    uint16_t address = self->opcode & 0x0FFFu;
    self->pc = self->registers[0] + address;
}

static void op_Cxkk(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t byte = self->opcode & 0x00FFu;
    uint8_t rand_byte = rand() % 255;

    self->registers[ix] = rand_byte & byte;
}

static void op_Dxyn(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t iy = yreg_idx(self->opcode);
    uint8_t ht = self->opcode & 0x000Fu;

    // Wrap around the screen boundary
    uint8_t pos_x = self->registers[ix] % SCR_WD;
    uint8_t pos_y = self->registers[iy] % SCR_HT;

    self->registers[15] = 0;

    for (int row = 0; row < ht; ++row)
    {
        uint8_t sprite_byte = self->memory[self->idx_ptr + row];

        for (int col = 0; col < 8; ++col)
        {
            uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
            int vid_mem_idx = (row + pos_y) * SCR_WD + (col + pos_x);

            if (sprite_pixel)
            {
                if (self->video_memory[vid_mem_idx] == 0xFFFFFFFF)
                {
                    self->registers[15] = 1;
                }

                self->video_memory[vid_mem_idx] ^= 0xFFFFFFFF;
            }
        }
    }
}

static void op_Ex9E(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    if (self->keypad[self->registers[ix]])
    {
        self->pc += 2;
    }
}

static void op_ExA1(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    if (!self->keypad[self->registers[ix]])
    {
        self->pc += 2;
    }
}

static void op_Fx07(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    self->registers[ix] = self->delay_timer;
}

static void op_Fx0A(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    for (int i = 0; i < 16; ++i)
    {
        if (self->keypad[i])
        {
            self->registers[ix] = i;
            return;
        }
    }

    self->pc -= 2;
}

static void op_Fx15(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    self->delay_timer = self->registers[ix];
}

static void op_Fx18(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    self->sound_timer = self->registers[ix];
}

static void op_Fx1E(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    self->idx_ptr += self->registers[ix];
}

static void op_Fx29(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    self->idx_ptr = FONTSET_START_ADDRESS + 5 * self->registers[ix];
}

static void op_Fx33(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);
    uint8_t num = self->registers[ix];

    uint8_t val = num;

    self->memory[self->idx_ptr + 2] = val % 10;
    val /= 10;
    self->memory[self->idx_ptr + 1] = val % 10;
    val /= 10;
    self->memory[self->idx_ptr + 0] = val % 10;
}

static void op_Fx55(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    for (int i = 0; i <= ix; ++i)
    {
        self->memory[self->idx_ptr + i] = self->registers[i];
    }
}

static void op_Fx65(Chip8 *self)
{
    uint8_t ix = xreg_idx(self->opcode);

    for (int i = 0; i <= ix; ++i)
    {
        self->registers[i] = self->memory[self->idx_ptr + i];
    }
}
