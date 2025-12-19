#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "Mapper.h"

class Cartridge {
public:
    enum MIRROR {
        HORIZONTAL,
        VERTICAL,
        ONESCREEN_LO,
        ONESCREEN_HI,
    } mirror = HORIZONTAL;

public:
    Cartridge(const std::string& sFileName);
    ~Cartridge();

public:
    bool cpuRead(uint16_t addr, uint8_t& data);
    bool cpuWrite(uint16_t addr, uint8_t data);
    bool ppuRead(uint16_t addr, uint8_t& data);
    bool ppuWrite(uint16_t addr, uint8_t data);

public:
    uint8_t nMapperID = 0;
    uint8_t nPRGBanks = 0;
    uint8_t nCHRBanks = 0;
    std::vector<uint8_t> vPRGMemory;
    std::vector<uint8_t> vCHRMemory;

    std::unique_ptr<Mapper> pMapper;
};