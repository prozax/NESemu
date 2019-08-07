#ifndef NESEMU_CPU6502_H
#define NESEMU_CPU6502_H


#include <cstdint>
#include <string>
#include <iostream>
#include <bitset>
#include "RAM.h"

class CPU6502 {
public:
    CPU6502(RAM *ram);
    void clock();
    std::string debug();

private:
    RAM* _ram;
    uint8_t _a, _x, _y;
    uint16_t _programm_counter;
    uint8_t _stack_pointer;
    uint8_t _processor_status;
    uint8_t cycles_left;
    std::string _debug_opcode_string;
    uint8_t _debug_opcode_byte;
    uint16_t _debug_programm_counter;

    int decode();
    uint16_t get_address(uint16_t);
    int execute(uint8_t, uint16_t);
    void non_maskable_interrupt();


    void set_negative_flag(uint8_t);
    void set_overflow_flag(bool);
    void set_break_flag(uint8_t);
    void set_decimal_flag(uint8_t);
    void set_irq_disable_flag(uint8_t);
    void set_zero_flag(uint8_t);
    void set_carry_flag(bool);

    uint8_t get_negative_flag();
    uint8_t get_overflow_flag();
    uint8_t get_break_flag();
    uint8_t get_decimal_flag();
    uint8_t get_irg_disable_flag();
    uint8_t get_zero_flag();
    uint8_t get_carry_flag();

    void stack_push(uint8_t);
    uint8_t stack_pull();
};


#endif //NESEMU_CPU6502_H
