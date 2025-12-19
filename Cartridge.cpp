#include "Cartridge.h"
#include <fstream>
#include <iostream>

Cartridge::Cartridge(const std::string& sFileName) {
    struct sHeader {
        char name[4];
        uint8_t prg_rom_chunks;
        uint8_t chr_rom_chunks;
        uint8_t mapper1;
        uint8_t mapper2;
        uint8_t prg_ram_size;
        uint8_t tv_system1;
        uint8_t tv_system2;
        char unused[5];
    } header;

    std::ifstream ifs;
    ifs.open(sFileName, std::ifstream::binary);
    if (!ifs.is_open()) {
        std::cerr << "Fatal Error: Could not open ROM file '" << sFileName << "'." << std::endl;
        return;
    }

    ifs.read((char*)&header, sizeof(sHeader));

    if (header.mapper1 & 0x04)
        ifs.seekg(512, std::ios_base::cur);

    nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
    mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

    nPRGBanks = header.prg_rom_chunks;
    vPRGMemory.resize((size_t)nPRGBanks * 16384);
    ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

    nCHRBanks = header.chr_rom_chunks;
    if (nCHRBanks == 0) {
        vCHRMemory.resize(8192);
    }
    else {
        vCHRMemory.resize((size_t)nCHRBanks * 8192);
        ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
    }

    ifs.close();

    pMapper = Mapper::createMapper(nMapperID, this);
}

Cartridge::~Cartridge() {}

bool Cartridge::cpuRead(uint16_t addr, uint8_t& data) {
    return pMapper->cpuRead(addr, data);
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data) {
    return pMapper->cpuWrite(addr, data);
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t& data) {
    return pMapper->ppuRead(addr, data);
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data) {
    return pMapper->ppuWrite(addr, data);
}