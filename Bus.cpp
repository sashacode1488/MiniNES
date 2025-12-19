#include "Bus.h"

Bus::Bus() {
    cpu.connectBus(this);
    apu.connectBus(this);
    for (auto& i : cpuRam) i = 0x00;
    controller[0] = controller[1] = 0x00;
    controller_state[0] = controller_state[1] = 0x00;
}

Bus::~Bus() {}

void Bus::cpuWrite(uint16_t addr, uint8_t data) {
    if (cart->cpuWrite(addr, data)) {
        
    }
    else if (addr <= 0x1FFF) {
        cpuRam[addr & 0x07FF] = data;
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF) {
        ppu.cpuWrite(addr & 0x0007, data);
    }
    else if (addr == 0x4014) {
        cpu.dma_page = data;
        cpu.dma_addr = 0x00;
        cpu.dma_transfer = true;
    }
    else if (addr >= 0x4000 && addr <= 0x4013 || addr == 0x4015 || addr == 0x4017) {
        apu.write(addr, data);
    }
    else if (addr >= 0x4016 && addr <= 0x4017) {
        controller_state[addr & 0x0001] = controller[addr & 0x0001];
    }
}

uint8_t Bus::cpuRead(uint16_t addr, bool bReadOnly) {
    uint8_t data = 0x00;

    if (cart->cpuRead(addr, data)) {
        
    }
    else if (addr <= 0x1FFF) {
        data = cpuRam[addr & 0x07FF];
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF) {
        data = ppu.cpuRead(addr & 0x0007, bReadOnly);
    }
    else if (addr == 0x4015) {
        data = apu.read(addr);
    }
    else if (addr >= 0x4016 && addr <= 0x4017) {
        data = (controller_state[addr & 0x0001] & 0x80) > 0;
        controller_state[addr & 0x0001] <<= 1;
    }
    else {
        data = open_bus_data;
    }

    open_bus_data = data;
    return data;
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge) {
    this->cart = cartridge;
    ppu.connectCartridge(this->cart);
}

void Bus::reset() {
    cpu.reset();
    ppu.reset();
    apu.reset();
    nSystemClockCounter = 0;
}

void Bus::clock() {
    
    ppu.clock();

    
    apu.clock();

    
    if (cpu.dma_transfer) {
        if (dma_dummy) {
            if (nSystemClockCounter % 2 == 1) {
                dma_dummy = false;
            }
        }
        else {
            if (nSystemClockCounter % 2 == 0) {
                dma_data = cpuRead((uint16_t)cpu.dma_page << 8 | cpu.dma_addr);
            }
            else {
                ppu.pOAM[cpu.dma_addr] = dma_data;
                cpu.dma_addr++;
                if (cpu.dma_addr == 0x00) {
                    cpu.dma_transfer = false;
                    dma_dummy = true;
                }
            }
        }
    }
    else if (nSystemClockCounter % 3 == 0) {
        
        cpu.clock();
    }

    
    if (cart->pMapper->irqState()) {
        cart->pMapper->irqClear();
        cpu.irq();
    }

    
    if (apu.getIRQ()) {
        cpu.irq();
    }

    
    if (ppu.nmi) {
        ppu.nmi = false;
        cpu.nmi();
    }

    nSystemClockCounter++;
}