#ifndef NESEMU_RAM_H
#define NESEMU_RAM_H

#include <cstdint>
#include "Cartridge.h"

class RAM {
public:
    RAM(Cartridge *cart): cart(cart) {};
    //uint8_t read(uint16_t address);
    //void write(uint8_t value, uint16_t address);
    uint8_t read_cpu(uint16_t address);
    void write_cpu(uint16_t address, uint8_t value);
    uint8_t read_ppu(uint16_t address);
    void write_ppu(uint16_t address, uint8_t value);

private:
    uint8_t ram_cpu[2048];
    uint8_t vram[2048];
    Cartridge* cart;

};


#endif //NESEMU_RAM_H
