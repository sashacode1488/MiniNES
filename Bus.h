#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include "Cpu.h"
#include "Ppu.h"
#include "Cartridge.h"
#include "APU.h"

class Bus {
public:
    Bus();
    ~Bus();
public:
    Cpu cpu;
    Ppu ppu;
    APU apu;
    std::array<uint8_t, 2048> cpuRam;
    std::shared_ptr<Cartridge> cart;
    uint32_t nSystemClockCounter = 0;
    uint8_t controller[2];
private:
    uint8_t controller_state[2];
    uint8_t dma_data = 0x00;     
    bool dma_dummy = true;       

    
    uint8_t open_bus_data = 0x00;

public:
    void cpuWrite(uint16_t addr, uint8_t data);
    uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);
    void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);
    void reset();
    void clock();
};