#ifndef NESEMU_CARTRIDGE_H
#define NESEMU_CARTRIDGE_H


#include <cstdint>
#include <string>
#include <fstream>
#include <istream>
#include <cstring>

class Cartridge {
public:
    Cartridge();
    Cartridge(std::string file_name);

    virtual ~Cartridge();

    int read_file(std::string file_name);
    uint8_t read_rom(uint16_t address);
    void write_rom(uint16_t address, uint8_t value);
    uint8_t read_ram(uint16_t address);
    void write_ram(uint16_t address, uint8_t value);
    uint8_t read_vram(uint16_t address);

private:
    int prg_rom_size;
    int pgr_ram_backed;
    int pgr_ram_not_backed;
    int chr_rom_size;
    int chr_rom_backed;
    int chr_rom_not_backed;
    int file_version;
    int mapper;
    int submapper;
    bool chr_ram;
    bool four_screen_mode;
    bool trainer_available;
    bool sram_battery_backed;
    bool horizontal_mirroring;
    bool playerchoice_10;
    bool vs_unisystem;
    bool tv_system; //0 - NTSC, 1 - PAL

    uint8_t* prg_rom;
    uint8_t* prg_ram;
    uint8_t* chr_rom;
    uint8_t* trainer;
};


#endif //NESEMU_CARTRIDGE_H
