#include <sstream>
#include <iomanip>
#include "CPU6502.h"

void CPU6502::clock() {
    //if(cycles_left == 0)
        decode();

    //cycles_left--;
}

int CPU6502::decode() {
    uint8_t opcode = _ram->read_cpu(_programm_counter);
    _debug_opcode_byte = opcode;
    _debug_programm_counter = _programm_counter;
    _programm_counter++;
    execute(opcode, get_address(opcode));
    return 0;
}

int CPU6502::execute(uint8_t opcode, uint16_t address) {
    uint16_t result = 0;

    switch(opcode) {
        /** ADC - Add to Accumulator with Carry **/
        case 0x71: case 0x6D: case 0x7D: case 0x79: case 0x69: case 0x61: case 0x65: case 0x75:
            result = _ram->read_cpu(address) + _a + get_carry_flag();
            set_carry_flag((result & 0x100) > 0);

            if((_ram->read_cpu(address) ^ result) & (_a ^ result) & 0x80)
                set_overflow_flag(1);
            else
                set_overflow_flag(0);

            _a = (uint8_t) result;
            set_zero_flag(_a);
            set_negative_flag(_a);
            _debug_opcode_string = "ADC";
            break;

        /** ANC **/
        case 0x0B: case 0x2B:
            break;

        /** AND - AND Memory with Accumulator **/
        case 0x31: case 0x2D: case 0x3D: case 0x39: case 0x29: case 0x21: case 0x25: case 0x35:
            _a = _a & _ram->read_cpu(address);
            set_zero_flag(_a);
            set_negative_flag(_a);
            _debug_opcode_string = "AND";
            break;

        /** ANE **/
        case 0x8B:
            _debug_opcode_string = "ANE";
            break;

        /** ARR **/
        case 0x6B:
            _debug_opcode_string = "ARR";
            break;

        /** ASL - Arithmatic Shift Left (Accumulator) **/
        case 0x0A:
            set_carry_flag((_a & 0x80) > 0);
            _a = _a << 1;
            set_zero_flag(_a);
            set_negative_flag(_a);
            _debug_opcode_string = "ASL";
            break;

        /** ASL - Arithmatic Shift Left (Memory) **/
        case 0x06: case 0x16: case 0x0E: case 0x1E:
            set_carry_flag((_ram->read_cpu(address) & 0x80) > 0);
            result = _ram->read_cpu(address) << 1;
            set_zero_flag(result);
            set_negative_flag(result);
            _ram->write_cpu(address, result);
            _debug_opcode_string = "ASL";
            break;

        /** ASR **/
        case 0x4B:
            _debug_opcode_string = "ASR";
            break;

        /** BCC - Branch on Carry Clear **/
        case 0x90:
            if(get_carry_flag() == 0) {
                _programm_counter = address;
            }
            _debug_opcode_string = "BCC";
            break;

        /** BCS - Branch on Carry Set **/
        case 0xB0:
            if(get_carry_flag() == 1) {
                _programm_counter = address;
            }
            _debug_opcode_string = "BCS";
            break;

        /** BEQ - Branch Zero Set **/
        case 0xF0:
            if(get_zero_flag() == 1) {
                _programm_counter = address;
            }
            _debug_opcode_string = "BEQ";
            break;

        /** BIT - Test Bits in Memory with Accumulator **/
        case 0x24: case 0x2C:
            set_overflow_flag((_ram->read_cpu(address) & 0x40) > 0);
            set_negative_flag(_ram->read_cpu(address));
            set_zero_flag(_a & _ram->read_cpu(address));
            _debug_opcode_string = "BIT";
            break;

        /** BMI - Branch on Result Minus **/
        case 0x30:
            if(get_negative_flag() == 1) {
                _programm_counter = address;
            }
            _debug_opcode_string = "BMI";
            break;

        /** BNE - Branch on Z reset **/
        case 0xD0:
            if(get_zero_flag() == 0) {
                _programm_counter = address;
            }
            _debug_opcode_string = "BNE";
            break;

        /** BPL - Branch on Result Plus (or Positive) **/
        case 0x10:
            if(get_negative_flag() == 0) {
                _programm_counter = address;
            }
            _debug_opcode_string = "BPL";
            break;

        /** BRK - Force a Break **/
        case 0x00:
            stack_push(_programm_counter >> 8);
            stack_push(_programm_counter);
            stack_push(_processor_status);
            set_break_flag(1);
            set_irq_disable_flag(1);
            _programm_counter = address;
            _debug_opcode_string = "BRK";
            break;

        /** BVC - Branch on Overflow Clear **/
        case 0x50:
            if(get_overflow_flag() == 0) {
                _programm_counter = address;
            }
            _debug_opcode_string = "BVC";
            break;

        /** BVS - Branch on Overflow Set **/
        case 0x70:
            if(get_overflow_flag() == 1) {
                _programm_counter = address;
            }
            _debug_opcode_string = "BVS";
            break;

        /** CLC - Clear Carry Flag **/
        case 0x18:
            set_carry_flag(0);
            _debug_opcode_string = "CLC";
            break;

        /** CLD - Clear Decimal Mode **/
        case 0xD8:
            set_decimal_flag(0);
            _debug_opcode_string = "CLD";
            break;

        /** CLI - Clear Interrupt Disable **/
        case 0x58:
            set_irq_disable_flag(0);
            _debug_opcode_string = "CLI";
            break;

        /** CLV - Clear Overflow Flag **/
        case 0xB8:
            set_overflow_flag(0);
            _debug_opcode_string = "CLV";
            break;

        /** CMP - Compare Memory and Accumulator **/
        case 0xC9: case 0xC5: case 0xD5: case 0xCD: case 0xDD: case 0xD9: case 0xC1: case 0xD1:
            set_negative_flag(_a - _ram->read_cpu(address));
            set_zero_flag(_a - _ram->read_cpu(address));
            set_carry_flag(_a >= _ram->read_cpu(address));
            _debug_opcode_string = "CMP";
            break;

        /** CPX - Compare Memory and X register **/
        case 0xE0: case 0xE4: case 0xEC:
            set_negative_flag(_x - _ram->read_cpu(address));
            set_zero_flag(_x - _ram->read_cpu(address));
            set_carry_flag(_x >= _ram->read_cpu(address));
            _debug_opcode_string = "CPX";
            break;

        /** CPY - Compare Memory and Y register **/
        case 0xC0: case 0xC4: case 0xCC:
            set_negative_flag(_y - _ram->read_cpu(address));
            set_zero_flag(_y - _ram->read_cpu(address));
            set_carry_flag(_y >= _ram->read_cpu(address));
            _debug_opcode_string = "CPY";
            break;

        /** CDP **/
        case 0xD3: case 0xCF: case 0xDF: case 0xDB: case 0xC3: case 0xC7: case 0xD7:
            _debug_opcode_string = "CDP";
            break;

        /** DEC - Decrement Memory by One **/
        case 0xC6: case 0xD6: case 0xCE: case 0xDE:
            result = _ram->read_cpu(address) - 1;
            set_negative_flag(result);
            set_zero_flag(result);
            _ram->write_cpu(address, result);
            _debug_opcode_string = "DEC";
            break;

        /** DEX - Decrement X **/
        case 0xCA:
            _x--;
            set_negative_flag(_x);
            set_zero_flag(_x);
            _debug_opcode_string = "DEX";
            break;

        /** DEY - Decrement Y **/
        case 0x88:
            _y--;
            set_negative_flag(_y);
            set_zero_flag(_y);
            _debug_opcode_string = "DEY";
            break;

        /** EOR - Exclusive-OR Memory with Accumulator **/
        case 0x51: case 0x4D: case 0x5D: case 0x59: case 0x49: case 0x45: case 0x55:
            _a ^= _ram->read_cpu(address);
            set_negative_flag(_a);
            set_zero_flag(_a);
            _debug_opcode_string = "EOR";
            break;

        /** INC - Increment Memory by one **/
        case 0xEE: case 0xFE: case 0xE6: case 0xF6:
            result = _ram->read_cpu(address) + 1;
            set_negative_flag(result);
            set_zero_flag(result);
            _ram->write_cpu(address, result);
            _debug_opcode_string = "INC";
            break;

        /** INX - Increment X by one **/
        case 0xE8:
            _x++;
            set_negative_flag(_x);
            set_zero_flag(_x);
            _debug_opcode_string = "INX";
            break;

        /** INY - Increment Y by one **/
        case 0xC8:
            _y++;
            set_negative_flag(_y);
            set_zero_flag(_y);
            _debug_opcode_string = "INY";
            break;

        /** ISB **/
        case 0xF3: case 0xEF: case 0xFF: case 0xFB: case 0xE3: case 0xE7: case 0xF7:
            _debug_opcode_string = "ISB";
            break;

        /** JAM **/
        case 0x02: case 0x12: case 0x22: case 0x32: case 0x42: case 0x52: case 0x62: case 0x72: case 0x92: case 0xB2:
        case 0xD2: case 0xF2:
            _debug_opcode_string = "JAM";
            break;

        /** JMP - Jump **/
        case 0x4C: case 0x6C:
            _programm_counter = address;
            _debug_opcode_string = "JMP";
            break;

        /** JSR - Jump to subroutine **/
        case 0x20:
            _programm_counter--;
            stack_push(_programm_counter >> 8);
            stack_push(_programm_counter);
            _programm_counter = address;
            _debug_opcode_string = "JSR";
            break;

        /** LAE **/
        case 0xBB:
            _debug_opcode_string = "LAE";
            break;

        /** LAX **/
        case 0xB3: case 0xAF: case 0xBF: case 0xA3: case 0xA7: case 0xB7:
            _debug_opcode_string = "LAX";
            break;

        /** LDA - Load Accumulator with memory **/
        case 0xB1: case 0xAD: case 0xBD: case 0xB9: case 0xA9: case 0xA1: case 0xA5: case 0xB5:
            _a = _ram->read_cpu(address);
            set_negative_flag(_a);
            set_zero_flag(_a);
            _debug_opcode_string = "LDA";
            break;

        /** LDX - Load X with Memory **/
        case 0xAE: case 0xBE: case 0xA2: case 0xA6: case 0xB6:
            _x = _ram->read_cpu(address);
            set_negative_flag(_x);
            set_zero_flag(_x);
            _debug_opcode_string = "LDX";
            break;

        /** LDY - Load Y with Memory **/
        case 0xAC: case 0xBC: case 0xA0: case 0xA4: case 0xB4:
            _y = _ram->read_cpu(address);
            set_negative_flag(_y);
            set_zero_flag(_y);
            _debug_opcode_string = "LDY";
            break;

        /** LSR - Logical Shift Right (Accumulator) **/
        case 0x4A:
            set_carry_flag((_a & 0x01) > 0);
            _a = _a >> 1;
            set_zero_flag(_a);
            set_negative_flag(0);
            _debug_opcode_string = "LSR";
            break;

        /** LSR - Logical Shift Right (Memory) **/
        case 0x46: case 0x56: case 0x4E: case 0x5E:
            set_carry_flag((_ram->read_cpu(address) & 0x01) > 0);
            result = _ram->read_cpu(address) >> 1;
            set_zero_flag(result);
            set_negative_flag(0);
            _ram->write_cpu(address, result);
            _debug_opcode_string = "LSR";
            break;

        /** LXA **/
        case 0xAB:
            _debug_opcode_string = "LXA";
            break;

        /** NOP - No Operation **/
        case 0x0C: case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC: case 0x80: case 0x82: case 0x89:
        case 0xC2: case 0xE2: case 0xEA: case 0x1A: case 0x3A: case 0x5A: case 0x7A: case 0xDA: case 0xFA: case 0x04:
        case 0x44: case 0x64: case 0x14: case 0x34: case 0x54: case 0x74: case 0xD4: case 0xF4:
            _debug_opcode_string = "NOP";
            break;

        /** ORA - OR Memory with Accumulator **/
        case 0x11: case 0x0D: case 0x1D: case 0x19: case 0x09: case 0x01: case 0x05: case 0x15:
            _a = _a | _ram->read_cpu(address);
            set_negative_flag(_a);
            set_zero_flag(_a);
            _debug_opcode_string = "ORA";
            break;

        /** PHA - Push Accumulator on Stack **/
        case 0x48:
            stack_push(_a);
            _debug_opcode_string = "PHA";
            break;

        /** PHP - Push Processor Status on Stack **/
        case 0x08:
            stack_push(_processor_status);
            _debug_opcode_string = "PHP";
            break;

        /** PLA - Pull Accumulator from Stack **/
        case 0x68:
            _a = stack_pull();
            set_negative_flag(_a);
            set_zero_flag(_a);
            _debug_opcode_string = "PLA";
            break;

        /** PLP - Pull Processor Status from Stack **/
        case 0x28:
            _processor_status = stack_pull();
            _debug_opcode_string = "PLP";
            break;

        /** RLA **/
        case 0x33: case 0x2F: case 0x3F: case 0x3B: case 0x23: case 0x27: case 0x37:
            _debug_opcode_string = "RLA";
            break;

        /** ROL - Rotate Left (Accumulator) **/
        case 0x2A:
            set_carry_flag((0x80 & _a) > 0);
            _a = _a << 1;
            _a = get_carry_flag() ? _a & 0x01 : _a;
            set_negative_flag(_a);
            set_zero_flag(_a);
            _debug_opcode_string = "ROL";
            break;

        /** ROL - Rotate Left (Memory) **/
        case 0x2E: case 0x3E: case 0x26: case 0x36:
            result = _ram->read_cpu(address) << 1;
            result = get_carry_flag() ? result & 0x01 : result;
            set_carry_flag((0x100 & result) > 0);
            set_negative_flag(result);
            set_zero_flag(result);
            _ram->write_cpu(address, result);
            _debug_opcode_string = "ROL";
            break;

        /** ROR - Rotate Right (Accumulator) **/
        case 0x6A:
            result = get_carry_flag() ? 0x100 & _a : _a;
            set_carry_flag((result & 0x01) == 1);
            result = result >> 1;
            _a = result;
            set_negative_flag(_a);
            set_zero_flag(_a);
            _debug_opcode_string = "ROR";
            break;

        /** ROR - Rotate Right (Memory) **/
        case 0x6E: case 0x7E: case 0x66: case 0x76:
            result = get_carry_flag() ? 0x100 & _ram->read_cpu(address) : _ram->read_cpu(address);
            set_carry_flag((result & 0x01) == 1);
            result = result >> 1;
            set_negative_flag(result);
            set_zero_flag(result);
            _ram->write_cpu(address, result);
            _debug_opcode_string = "ROR";
            break;

        /** RRA **/
        case 0x73: case 0x6F: case 0x7F: case 0x7B: case 0x63: case 0x67: case 0x77:
            _debug_opcode_string = "RRA";
            break;

        /** RTI - Return from Interrupt **/
        case 0x40:
            _programm_counter = stack_pull();
            _programm_counter = stack_pull() | (stack_pull() << 8);
            _debug_opcode_string = "RTI";
            break;

        /** RTS - Return from Subroutine **/
        case 0x60:
            _programm_counter = stack_pull() | (stack_pull() << 8);
            _programm_counter++;
            _debug_opcode_string = "RTS";
            break;

        /** SAX **/
        case 0x8F: case 0x83: case 0x87: case 0x97:
            _debug_opcode_string = "SAX";
            break;

        /** SBC - Subtract from Accumulator with Carry **/
        case 0xF1: case 0xED: case 0xFD: case 0xF9: case 0xE9: case 0xEB: case 0xE1: case 0xE5: case 0xF5:
            result = _a - _ram->read_cpu(address) - (get_carry_flag() ? 0 : 1);

            set_overflow_flag(((_ram->read_cpu(address) ^ _a) & (_a ^ result) & 0x80) != 0);

            _a = result;
            set_carry_flag(result < 0x100);
            set_zero_flag(_a);
            _debug_opcode_string = "SBC";
            break;

        /** SBX **/
        case 0xCB:
            _debug_opcode_string = "SBX";
            break;

        /** SEC - Set Carry Flag **/
        case 0x38:
            set_carry_flag(true);
            _debug_opcode_string = "SEC";
            break;

        /** SED - Set Decimal Mode **/
        case 0xF8:
            set_decimal_flag(1);
            _debug_opcode_string = "SED";
            break;

        /** SEI - Set Interrupt Disable **/
        case 0x78:
            set_irq_disable_flag(1);
            _debug_opcode_string = "SEI";
            break;

        /** SHA **/
        case 0x93: case 0x9F:
            _debug_opcode_string = "SHA";
            break;

        /** SHS **/
        case 0x9B:
            _debug_opcode_string = "SHS";
            break;

        /** SHX **/
        case 0x9E:
            _debug_opcode_string = "SHX";
            break;

        /** SHY **/
        case 0x9C:
            _debug_opcode_string = "SHY";
            break;

        /** SLO **/
        case 0x13: case 0x0F: case 0x1F: case 0x1B: case 0x03: case 0x07: case 0x17:
            _debug_opcode_string = "SLO";
            break;

        /** SRE **/
        case 0x53: case 0x4F: case 0x5F: case 0x5B: case 0x43: case 0x47: case 0x57:
            _debug_opcode_string = "SRE";
            break;

        /** STA - Store Accumulator in Memory **/
        case 0x91: case 0x8D: case 0x9D: case 0x99: case 0x81: case 0x85: case 0x95:
            _ram->write_cpu(address, _a);
            _debug_opcode_string = "STA";
            break;

        /** STX - Store X in Memory **/
        case 0x8E: case 0x86: case 0x96:
            _ram->write_cpu(address, _x);
            _debug_opcode_string = "STX";
            break;

        /** STY - Store Y in Memory **/
        case 0x8C: case 0x84: case 0x94:
            _ram->write_cpu(address, _y);
            _debug_opcode_string = "STY";
            break;

        /** TAX - Transfer Accumulator to X **/
        case 0xAA:
            _x = _a;
            set_negative_flag(_x);
            set_zero_flag(_x);
            _debug_opcode_string = "TAX";
            break;

        /** TAY - Transfer Accumulator to Y **/
        case 0xA8:
            _y = _a;
            set_negative_flag(_y);
            set_zero_flag(_y);
            _debug_opcode_string = "TAY";
            break;

        /** TSX - Transfer Stack to X **/
        case 0xBA:
            _x = _stack_pointer;
            set_negative_flag(_x);
            set_zero_flag(_x);
            _debug_opcode_string = "TSX";
            break;

        /** TXA - Transfer X to Accumulator **/
        case 0x8A:
            _a = _x;
            set_negative_flag(_x);
            set_zero_flag(_x);
            _debug_opcode_string = "TXA";
            break;

        /** TXS - Transfer X to Stack **/
        case 0x9A:
            _stack_pointer = _x;
            _debug_opcode_string = "TXS";
            break;

        /** TYA - Transfer Y to Accumulator **/
        case 0x98:
            _a = _y;
            set_negative_flag(_a);
            set_zero_flag(_a);
            _debug_opcode_string = "TYA";
            break;
    }
    return 0;
}

uint16_t CPU6502::get_address(uint16_t opcode) {
    uint16_t address = 0;
    switch(opcode) {
        /** Immediate Addressing **/
        case 0x69: case 0x0B: case 0x2B: case 0x29: case 0x8B: case 0x6B: case 0x4B: case 0xC9: case 0xE0:
        case 0xC0: case 0x49: case 0xA9: case 0xA2: case 0xA0: case 0xAB: case 0x80: case 0x82: case 0x89: case 0xC2:
        case 0xE2: case 0x09: case 0xE9: case 0xEB: case 0xCB:
            address = _programm_counter;
            _programm_counter++;
            break;

            /** Absolute Addressing **/
        case 0x6D: case 0x2D: case 0x0E: case 0x2C: case 0xCD: case 0xEC: case 0xCC: case 0xCF: case 0xCE: case 0x4D:
        case 0xEE: case 0xEF: case 0x4C: case 0x20: case 0xAF: case 0xAD: case 0xAE: case 0xAC: case 0x4E: case 0x0C:
        case 0x0D: case 0x2F: case 0x2E: case 0x6E: case 0x6F: case 0x8F: case 0xED: case 0x0F: case 0x4F: case 0x8D:
        case 0x8E: case 0x8C:

            address = _ram->read_cpu(_programm_counter) | (_ram->read_cpu(_programm_counter + 1) << 8);
            _programm_counter += 2;
            break;

            /** Zero Page Addressing **/
        case 0x65: case 0x25: case 0x06: case 0x24: case 0xC5: case 0xE4: case 0xC4: case 0xC7: case 0xC6: case 0x45:
        case 0xE6: case 0xE7: case 0xA7: case 0xA5: case 0xA6: case 0xA4: case 0x04: case 0x44: case 0x64: case 0x05:
        case 0x27: case 0x26: case 0x66: case 0x67: case 0x87: case 0xE5: case 0x07: case 0x47: case 0x85: case 0x86:
        case 0x84:
            address = _ram->read_cpu(_programm_counter);
            _programm_counter++;
            break;

            /** Implied Addressing **/
        case 0x18: case 0xD8: case 0x58: case 0xB8: case 0xCA: case 0x88: case 0xE8: case 0xC8: case 0x02: case 0x12:
        case 0x22: case 0x32: case 0x42: case 0x52: case 0x62: case 0x72: case 0x92: case 0xB2: case 0xD2: case 0xF2:
        case 0xEA: case 0x1A: case 0x3A: case 0x5A: case 0x7A: case 0xDA: case 0xFA: case 0x48: case 0x08: case 0x68:
        case 0x28: case 0x40: case 0x60: case 0x38: case 0xF8: case 0x78: case 0xAA: case 0xA8: case 0xBA: case 0x8A:
        case 0x9A: case 0x98:
            break;

            /** Indirect Addressing **/
        case 0x6C:
            address = _ram->read_cpu(_programm_counter) | (_ram->read_cpu(_programm_counter + 1) << 8);
            _programm_counter += 2;

            if(address & 0xFF == 0xFF) { //JMP bug
                address = _ram->read_cpu(address) | (_ram->read_cpu(address & 0xFF00) << 8);
            }
            else {
                address = _ram->read_cpu(address) | (_ram->read_cpu(address + 1) << 8);
            }
            break;


            /** Absolute Indexed X Addressing **/
        case 0x7D: case 0x3D: case 0x1E: case 0xDD: case 0xDF: case 0xDE: case 0x5D: case 0xFE: case 0xFF: case 0xBD:
        case 0xBC: case 0x46: case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC: case 0x1D: case 0x3F:
        case 0x3E: case 0x7E: case 0x7F: case 0xFD: case 0x93: case 0x9E: case 0x1F: case 0x5F: case 0x9D:
            address = _ram->read_cpu(_programm_counter) | (_ram->read_cpu(_programm_counter + 1) << 8);
            _programm_counter += 2;
            address += _x;
            break;

            /** Absolute Indexed Y Addressing **/
        case 0x79: case 0x39: case 0xD9: case 0xDB: case 0x59: case 0xFB: case 0xBB: case 0xBF: case 0xB9: case 0xBE:
        case 0x19: case 0x3B: case 0x7B: case 0xF9: case 0x9F: case 0x9B: case 0x9C: case 0x1B: case 0x5B: case 0x99:
            address = _ram->read_cpu(_programm_counter) | (_ram->read_cpu(_programm_counter + 1) << 8);
            _programm_counter += 2;
            address += _y;
            break;

            /** Zero Page Indexed X Addressing **/
        case 0x75: case 0x35: case 0x16: case 0xD5: case 0xD7: case 0xD6: case 0x55: case 0xF6: case 0xF7: case 0xB5:
        case 0xB4: case 0x56: case 0x14: case 0x34: case 0x54: case 0x74: case 0xD4: case 0xF4: case 0x15: case 0x37:
        case 0x36: case 0x76: case 0x77: case 0xF5: case 0x17: case 0x57: case 0x95: case 0x94:
            address = _ram->read_cpu(_programm_counter);
            _programm_counter++;
            address = (address + _x) & 0xFF; // address always has to be in zero page
            break;

            /** Zero Page Indexed Y Addressing **/
        case 0xB7: case 0xB6: case 0x97: case 0x96:
            address = _ram->read_cpu(_programm_counter);
            _programm_counter++;
            address = (address + _y) & 0xFF; // address always has to be in zero page
            break;

            /** Indexed Indirect Addressing (X) **/
        case 0x61: case 0x21: case 0xC1: case 0xC3: case 0x41: case 0xE3: case 0xA3: case 0xA1: case 0x01: case 0x23:
        case 0x63: case 0x83: case 0xE1: case 0x03: case 0x43: case 0x81:
            address = _ram->read_cpu(((_ram->read_cpu(_programm_counter) + _x) & 0xFF) | ((_ram->read_cpu(
                    _programm_counter) + _x + 1) & 0xFF) << 8);
            _programm_counter++;
            break;

            /** Indirect Indexed Addressing (Y) **/
        case 0x71: case 0x31: case 0xD1: case 0xD3: case 0x51: case 0xF3: case 0xB3: case 0xB1: case 0x11: case 0x33:
        case 0x73: case 0xF1: case 0x13: case 0x53: case 0x91:
            address = _ram->read_cpu(_ram->read_cpu(_programm_counter) | (_ram->read_cpu(_programm_counter + 1) << 8));
            _programm_counter++;
            address += _y;
            break;

            /** Relative Addressing **/
        case 0x90: case 0xB0: case 0xF0: case 0x30: case 0xD0: case 0x10: case 0x50: case 0x70:
            address = _programm_counter + 1 + _ram->read_cpu(_programm_counter);
            _programm_counter++;
            break;

            /** BREAK **/
        case 0x00:
            address = _ram->read_cpu(0xFFFE) | (_ram->read_cpu(0xFFFF) << 8);
            break;

        default:
            break;
    }
    return address;
}

void CPU6502::set_negative_flag(uint8_t t) {
    if((t & 0x80) > 0)
        _processor_status |= 0x80;
    else
        _processor_status &= 0x7F;
}

void CPU6502::set_overflow_flag(bool t) {
    if(t)
        _processor_status |= 0x40;
    else
        _processor_status &= 0xBF;
}

void CPU6502::set_break_flag(uint8_t t) {
    if(t > 0)
        _processor_status |= 0x10;
    else
        _processor_status &= 0xEF;
}

void CPU6502::set_decimal_flag(uint8_t t) {
    if(t > 0)
        _processor_status |= 0x08;
    else
        _processor_status &= 0xF7;
}

void CPU6502::set_irq_disable_flag(uint8_t t) {
    if(t > 0)
        _processor_status |= 0x04;
    else
        _processor_status &= 0xFB;
}

void CPU6502::set_zero_flag(uint8_t t) {
    if(t == 0)
        _processor_status |= 0x02;
    else
        _processor_status &= 0xFD;
}

void CPU6502::set_carry_flag(bool t) {
    if(t)
        _processor_status |= 0x01;
    else
        _processor_status &= 0xFE;
}

uint8_t CPU6502::get_negative_flag() {
    return (_processor_status & 0x80) >> 7;
}

uint8_t CPU6502::get_overflow_flag() {
    return (_processor_status & 0x40) >> 6;
}

uint8_t CPU6502::get_break_flag() {
    return (_processor_status & 0x10) >> 4;
}

uint8_t CPU6502::get_decimal_flag() {
    return (_processor_status & 0x08) >> 3;
}

uint8_t CPU6502::get_irg_disable_flag() {
    return (_processor_status & 0x04) >> 2;
}

uint8_t CPU6502::get_zero_flag() {
    return (_processor_status & 0x02) >> 1;
}

uint8_t CPU6502::get_carry_flag() {
    return (_processor_status & 0x01);
}

CPU6502::CPU6502(RAM *ram) : _ram(ram), cycles_left(0) {
    _processor_status = 0x34;
    _a = 0;
    _x = 0;
    _y = 0;
    _stack_pointer = 0xFD;
    _debug_opcode_byte = 0;
    //_programm_counter = ram->read_cpu(0xFFFC) | (ram->read_cpu(0xFFFD) << 8);
    _programm_counter = 0xC000;
}

void CPU6502::stack_push(uint8_t t) {
    _ram->write_cpu((uint16_t) 0x0100 | _stack_pointer--, t);
}

uint8_t CPU6502::stack_pull() {
    uint8_t result = _ram->read_cpu((uint16_t) 0x0100 | ++_stack_pointer);
    return result;
}

void CPU6502::non_maskable_interrupt() {
    stack_push(_programm_counter >> 8);
    stack_push(_programm_counter);
    stack_push(_processor_status);
    set_irq_disable_flag(1);
    _programm_counter = _ram->read_cpu(0xFFFA) | (_ram->read_cpu(0xFFFB) << 8);
}

std::string CPU6502::debug() {
    std::stringstream buf;
    buf << std::noshowbase << std::hex << std::uppercase << std::setw(4) << _debug_programm_counter << "\t";
    buf << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(_debug_opcode_byte) << "\t";
    buf << _debug_opcode_string << "\t\t";
    //buf<<"PS: "<<std::bitset<8>(_processor_status).to_string()<<"\t";
    buf<<"N"<<static_cast<unsigned int>(get_negative_flag())
    <<"V"<<static_cast<unsigned int>(get_overflow_flag())
    <<"D"<<static_cast<unsigned int>(get_decimal_flag())
    <<"I"<<static_cast<unsigned int>(get_irg_disable_flag())
    <<"Z"<<static_cast<unsigned int>(get_zero_flag())
    <<"C"<<static_cast<unsigned int>(get_carry_flag())<<"\t";
    buf << "A: " << static_cast<unsigned int>(_a) << "\t" << "X: " << static_cast<unsigned int>(_x) << "\t" << "Y: " << static_cast<unsigned int>(_y) << "\t";
    buf << "SP: " << static_cast<unsigned int>(_stack_pointer) << "\t";
    buf<<std::endl;
    return buf.str();
}
