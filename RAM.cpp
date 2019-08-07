#include "RAM.h"

uint8_t RAM::read_cpu(uint16_t address) {
    if(address < 0x800) {
        return ram_cpu[address];
    }
    else if(address >= 0x800 && address < 0x1000) { // 0x800 - 0x2000 are mirrors of 0x000 - 0x7FF
        return ram_cpu[address - 0x800];
    }
    else if(address >= 0x1000 && address < 0x1800) { // Mirrored
        return  ram_cpu[address - 0x1000];
    }
    else if(address >= 0x1800 && address < 0x2000) { // Mirrored
        return  ram_cpu[address - 0x1800];
    }
    else if(address >= 0x2000 && address < 0x2008) { // PPU
        //TODO
    }
    else if(address >= 0x4000 && address < 0x4018) { // I/O Ports
        //TODO
    }
    else if(address >= 0x5000 && address < 0x6000) { // Expansion Modules
        //TODO
    }
    else if(address >= 0x6000 && address < 0x8000) { // Cartridge RAM
        return cart->read_ram(address-0x6000);
    }
    else if(address >= 0x8000 && address < 0x10000) { // Cartridge ROM
        return cart->read_rom(address-0x8000);
    }

}

void RAM::write_cpu(uint16_t address, uint8_t value) {
    if(address < 0x800) {
        ram_cpu[address] = value;
    }
    else if(address >= 0x800 && address < 0x1000) {
        ram_cpu[address - 0x800] = value;
    }
    else if(address >= 0x1000 && address < 0x1800) {
        ram_cpu[address - 0x1000] = value;
    }
    else if(address >= 0x1800 && address < 0x2000) {
        ram_cpu[address - 0x1800] = value;
    }
    else if(address >= 0x2000 && address < 0x2008) { // I/O PPU
        //TODO
    }
    else if(address >= 0x4000 && address < 0x4018) { // I/O Ports
        //TODO
    }
    else if(address >= 0x5000 && address < 0x6000) { // Expansion Modules
        //TODO
    }
    else if(address >= 0x6000 && address < 0x8000) { // Cartridge RAM
        cart->write_ram(address-0x6000, value);
    }
    else if(address >= 0x8000 && address < 0x10000) { // Cartridge ROM
        cart->write_rom(address-0x8000, value);
    }
}

uint8_t RAM::read_ppu(uint16_t address) {
    if(address < 0x2000) {
        return cart->read_vram(address);
    }
    if(address >= 0x2000 && address < 0x2800) {
        return vram[address-0x2000];
    }
    else if(address >= 0x2800 && address < 0x3000) {
        return vram[address-0x2800];
    }
    return 0;
}

void RAM::write_ppu(uint16_t address, uint8_t value) {

}
