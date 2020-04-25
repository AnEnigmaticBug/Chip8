#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define SCR_WD 64
#define SCR_HT 32
#define STACK_SIZE 16

typedef struct Chip8
{
    uint16_t opcode;
    uint8_t registers[16];
    uint8_t sp;
    uint16_t pc;
    uint16_t idx_ptr;
    uint8_t keypad[16];
    uint8_t memory[4096];
    uint16_t stack[STACK_SIZE];
    uint32_t video_memory[SCR_WD * SCR_HT];
    uint8_t delay_timer;
    uint8_t sound_timer;
} Chip8;

#endif
