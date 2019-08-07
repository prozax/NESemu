#include "Cartridge.h"

Cartridge::Cartridge(): prg_rom(NULL), prg_ram(NULL), chr_rom(NULL), trainer(NULL) {
}


Cartridge::Cartridge(std::string file_name): prg_rom(NULL), prg_ram(NULL), chr_rom(NULL), trainer(NULL) {
    read_file(file_name);
}

int Cartridge::read_file(std::string file_name) {
    std::ifstream cart_file;
    char *memblock;

    cart_file.open(file_name);

    if(cart_file.is_open()) {
        cart_file.seekg(0, std::ios::beg);
        memblock = new char[16384];

        /** read constant "NES<EOF>" **/
        cart_file.read(memblock, 3);

        if (*memblock != 0x4E || *(memblock+1) != 0x45 || *(memblock+2) != 0x53) {
            delete[] memblock;
            cart_file.close();
            return -1;
        }

        cart_file.seekg(4, std::ios::beg);

        /** read PGR ROM size in 16KB units **/
        cart_file.read(memblock, 1);

        if (*memblock > 0) {
            prg_rom_size = *memblock;
        }

        /** read CHR_ROM size in 8KB units (0 indicates CHR_RAM) **/
        cart_file.read(memblock, 1);

        if (*memblock == 0) {
            chr_ram = true;
        }
        else {
            chr_ram = false;
            chr_rom_size = *memblock;
        }

        /** read Flags 6 **/
        cart_file.read(memblock, 1);
        mapper = *memblock >> 4;
        horizontal_mirroring = (*memblock & 0x01) != 0;
        four_screen_mode = (*memblock & 0x80) != 0;
        prg_ram = (*memblock & 0x02) ? new uint8_t[16384] : NULL;
        trainer_available = (*memblock & 0x04) != 0;

        /** read Flags 7 **/
        cart_file.read(memblock, 1);
        mapper = mapper | *memblock;
        vs_unisystem = (*memblock & 0x01) != 0;
        playerchoice_10 = (*memblock & 0x02) != 0;
        file_version = (*memblock & 0x0C) == 2 ? 2 : 1;

        /** read Flags 8 **/
        cart_file.read(memblock, 1);

        if(file_version == 1) {
            /** ignore Flags 8, read Flags 9 **/
            cart_file.read(memblock, 1);
            tv_system = (*memblock & 0x01) != 0;
        }
        else { /** NES 2.0 **/
            mapper = mapper | (*memblock & 0x0F) << 4;
            submapper = (*memblock & 0xF0) >> 4;

            /** read Flags 9 (Upper bits of ROM size) **/
            cart_file.read(memblock, 1);
            chr_rom_size = chr_rom_size | (*memblock & 0xF0) << 4;
            prg_rom_size = prg_rom_size | (*memblock & 0x0F) << 8;

            /** read Flags 10 (RAM size) **/
            cart_file.read(memblock, 1);
            pgr_ram_backed = (*memblock & 0xF0) >> 4;
            pgr_ram_not_backed = *memblock & 0x0F;

            /** read Flags 11 (VRAM size) **/
            cart_file.read(memblock, 1);
            chr_rom_backed = (*memblock & 0xF0) >> 4;
            chr_rom_not_backed = *memblock & 0x0F;

            /** read Flags 12 (TV system) **/
            cart_file.read(memblock, 1);
            tv_system = (*memblock & 0x01) != 0;

            //TODO: Flag 13
        }

        /** go to first byte after header **/
        cart_file.seekg(16, std::ios::beg);

        if(trainer_available) {
            trainer = new uint8_t[512];
            cart_file.read((char *) trainer, 512);
        }

        prg_rom = new uint8_t[prg_rom_size * 16384];
        cart_file.read((char *) prg_rom, prg_rom_size * 16384);
        chr_rom = new uint8_t[chr_rom_size * 8192];
        cart_file.read((char *) chr_rom, chr_rom_size * 8192);

        delete[] memblock;
        cart_file.close();
    }

    return 0;
}

uint8_t Cartridge::read_rom(uint16_t address) {
    if(prg_rom != NULL) {
        if(prg_rom_size == 1 && address >= 0x4000) {
            return *(prg_rom + address - 0x4000);
        }
        else {
            return *(prg_rom + address);
        }
    }

    return 0;
}

uint8_t Cartridge::read_ram(uint16_t address) {
    if(chr_rom != NULL) {
        return *(chr_rom + address);
    }
    return 0;
}

void Cartridge::write_ram(uint16_t address, uint8_t value) {

}

uint8_t Cartridge::read_vram(uint16_t address) {
    if(prg_ram != NULL) {
        return *(prg_ram + address);
    }
    return 0;
}

Cartridge::~Cartridge() {
    delete[] prg_rom;
    delete[] chr_rom;
    delete[] prg_ram;
    delete[] trainer;
}

void Cartridge::write_rom(uint16_t address, uint8_t value) {
    if(prg_rom != NULL) {
        *(prg_rom + address) = value;
    }
}
