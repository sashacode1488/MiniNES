#include "Mapper.h"
#include "Cartridge.h"
#include <iostream>

std::unique_ptr<Mapper> Mapper::createMapper(uint8_t mapperId, Cartridge* cart) {
    switch (mapperId) {
    case 0: return std::make_unique<Mapper000>(cart);
    case 1: return std::make_unique<Mapper001>(cart);
    case 2: return std::make_unique<Mapper002>(cart);
    case 3: return std::make_unique<Mapper003>(cart);
    case 4: return std::make_unique<Mapper004>(cart);
    case 5: return std::make_unique<Mapper005>(cart);
    case 6: return std::make_unique<Mapper006>(cart);
    case 7: return std::make_unique<Mapper007>(cart);
    case 16: return std::make_unique<Mapper016>(cart);
    case 18: return std::make_unique<Mapper018>(cart);
    case 19: return std::make_unique<Mapper019>(cart);
    default:
        std::cerr << "Unsupported mapper: " << (int)mapperId << std::endl;
        return std::make_unique<Mapper000>(cart);
    }
}


Mapper000::Mapper000(Cartridge* cart) : Mapper(cart) {}

bool Mapper000::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        uint32_t mapped_addr = addr & (cartridge->nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
        data = cartridge->vPRGMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper000::cpuWrite(uint16_t addr, uint8_t data) {
    return false;
}

bool Mapper000::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        data = cartridge->vCHRMemory[addr];
        return true;
    }
    return false;
}

bool Mapper000::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr <= 0x1FFF && cartridge->nCHRBanks == 0) {
        cartridge->vCHRMemory[addr] = data;
        return true;
    }
    return false;
}


Mapper001::Mapper001(Cartridge* cart) : Mapper(cart) {}

bool Mapper001::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        uint32_t bank_offset = prgBank * 0x8000;
        uint32_t mapped_addr = bank_offset + (addr & 0x7FFF);
        if (mapped_addr < cartridge->vPRGMemory.size()) {
            data = cartridge->vPRGMemory[mapped_addr];
            return true;
        }
    }
    return false;
}

bool Mapper001::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        writeRegister(addr, data);
        return true;
    }
    return false;
}

bool Mapper001::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        uint32_t bank_offset = (addr < 0x1000) ? chrBank0 * 0x1000 : chrBank1 * 0x1000;
        uint32_t mapped_addr = bank_offset + (addr & 0x0FFF);
        if (mapped_addr < cartridge->vCHRMemory.size()) {
            data = cartridge->vCHRMemory[mapped_addr];
            return true;
        }
    }
    return false;
}

bool Mapper001::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr <= 0x1FFF && cartridge->nCHRBanks == 0) {
        cartridge->vCHRMemory[addr] = data;
        return true;
    }
    return false;
}

void Mapper001::writeRegister(uint16_t addr, uint8_t data) {
    if (data & 0x80) {
        shiftRegister = 0x10;
        control |= 0x0C;
    }
    else {
        bool complete = shiftRegister & 1;
        shiftRegister >>= 1;
        shiftRegister |= (data & 1) << 4;

        if (complete) {
            switch (addr & 0x6000) {
            case 0x0000:
                control = shiftRegister & 0x1F;
                break;
            case 0x2000:
                chrBank0 = shiftRegister & 0x1F;
                break;
            case 0x4000:
                chrBank1 = shiftRegister & 0x1F;
                break;
            case 0x6000:
                prgBank = shiftRegister & 0x0F;
                break;
            }
            shiftRegister = 0x10;
        }
    }
}


Mapper002::Mapper002(Cartridge* cart) : Mapper(cart) {}

bool Mapper002::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        if (addr < 0xC000) {
            uint32_t mapped_addr = prgBank * 0x4000 + (addr & 0x3FFF);
            data = cartridge->vPRGMemory[mapped_addr];
        }
        else {
            uint32_t mapped_addr = (cartridge->nPRGBanks - 1) * 0x4000 + (addr & 0x3FFF);
            data = cartridge->vPRGMemory[mapped_addr];
        }
        return true;
    }
    return false;
}

bool Mapper002::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        prgBank = data & 0x0F;
        return true;
    }
    return false;
}

bool Mapper002::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        data = cartridge->vCHRMemory[addr];
        return true;
    }
    return false;
}

bool Mapper002::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr <= 0x1FFF && cartridge->nCHRBanks == 0) {
        cartridge->vCHRMemory[addr] = data;
        return true;
    }
    return false;
}


Mapper003::Mapper003(Cartridge* cart) : Mapper(cart) {}

bool Mapper003::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        uint32_t mapped_addr = addr & (cartridge->nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
        data = cartridge->vPRGMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper003::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        chrBank = data & 0x03;
        return true;
    }
    return false;
}

bool Mapper003::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        uint32_t mapped_addr = chrBank * 0x2000 + addr;
        data = cartridge->vCHRMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper003::ppuWrite(uint16_t addr, uint8_t data) {
    return false;
}


Mapper004::Mapper004(Cartridge* cart) : Mapper(cart) {

    prg_banks[0] = 0;
    prg_banks[1] = 1;


    for (int i = 0; i < 8; ++i) {
        chr_banks[i] = i;
    }


    target_register = 0;
    prg_bank_mode = false;
    chr_inversion = false;


    irq_counter = 0;
    irq_latch = 0;
    irq_enabled = false;
    irq_active = false;
    irq_reload = false;
}

bool Mapper004::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        uint32_t mapped_addr = 0;
        uint32_t bank_size = 0x2000;

        if (prg_bank_mode) {

            if (addr >= 0x8000 && addr <= 0x9FFF) {

                uint32_t bank = cartridge->nPRGBanks * 2 - 2;
                mapped_addr = bank * bank_size + (addr & 0x1FFF);
            }
            else if (addr >= 0xA000 && addr <= 0xBFFF) {
                mapped_addr = prg_banks[1] * bank_size + (addr & 0x1FFF);
            }
            else if (addr >= 0xC000 && addr <= 0xDFFF) {
                mapped_addr = prg_banks[0] * bank_size + (addr & 0x1FFF);
            }
            else {

                uint32_t bank = cartridge->nPRGBanks * 2 - 1;
                mapped_addr = bank * bank_size + (addr & 0x1FFF);
            }
        }
        else {

            if (addr >= 0x8000 && addr <= 0x9FFF) {
                mapped_addr = prg_banks[0] * bank_size + (addr & 0x1FFF);
            }
            else if (addr >= 0xA000 && addr <= 0xBFFF) {
                mapped_addr = prg_banks[1] * bank_size + (addr & 0x1FFF);
            }
            else if (addr >= 0xC000 && addr <= 0xDFFF) {

                uint32_t bank = cartridge->nPRGBanks * 2 - 2;
                mapped_addr = bank * bank_size + (addr & 0x1FFF);
            }
            else {

                uint32_t bank = cartridge->nPRGBanks * 2 - 1;
                mapped_addr = bank * bank_size + (addr & 0x1FFF);
            }
        }

        if (mapped_addr < cartridge->vPRGMemory.size()) {
            data = cartridge->vPRGMemory[mapped_addr];
            return true;
        }
    }
    return false;
}

bool Mapper004::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        if (addr <= 0x9FFF) {
            if (!(addr & 1)) {

                target_register = data & 0x07;
                prg_bank_mode = (data & 0x40);
                chr_inversion = (data & 0x80);
            }
            else {

                switch (target_register) {
                case 0: case 1:

                    chr_banks[target_register] = data & 0xFE;
                    break;
                case 2: case 3: case 4: case 5:

                    chr_banks[target_register] = data;
                    break;
                case 6:

                    prg_banks[0] = data & 0x3F;
                    break;
                case 7:

                    prg_banks[1] = data & 0x3F;
                    break;
                }
            }
        }
        else if (addr <= 0xBFFF) {
            if (!(addr & 1)) {

                switch (data & 0x01) {
                case 0: cartridge->mirror = Cartridge::VERTICAL; break;
                case 1: cartridge->mirror = Cartridge::HORIZONTAL; break;
                }
            }

        }
        else if (addr <= 0xDFFF) {
            if (!(addr & 1)) {

                irq_latch = data;
            }
            else {

                irq_reload = true;
            }
        }
        else {
            if (!(addr & 1)) {

                irq_enabled = false;
                irq_active = false;
            }
            else {

                irq_enabled = true;
            }
        }
        return true;
    }
    return false;
}

bool Mapper004::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        uint32_t mapped_addr = 0;

        if (chr_inversion) {

            if (addr <= 0x03FF) {
                mapped_addr = chr_banks[2] * 0x0400 + (addr & 0x03FF);
            }
            else if (addr <= 0x07FF) {
                mapped_addr = chr_banks[3] * 0x0400 + (addr & 0x03FF);
            }
            else if (addr <= 0x0BFF) {
                mapped_addr = chr_banks[4] * 0x0400 + (addr & 0x03FF);
            }
            else if (addr <= 0x0FFF) {
                mapped_addr = chr_banks[5] * 0x0400 + (addr & 0x03FF);
            }
            else if (addr <= 0x17FF) {

                mapped_addr = (chr_banks[0] & 0xFE) * 0x0400 + (addr & 0x07FF);
            }
            else {

                mapped_addr = (chr_banks[1] & 0xFE) * 0x0400 + (addr & 0x07FF);
            }
        }
        else {

            if (addr <= 0x07FF) {

                mapped_addr = (chr_banks[0] & 0xFE) * 0x0400 + (addr & 0x07FF);
            }
            else if (addr <= 0x0FFF) {

                mapped_addr = (chr_banks[1] & 0xFE) * 0x0400 + (addr & 0x07FF);
            }
            else if (addr <= 0x13FF) {
                mapped_addr = chr_banks[2] * 0x0400 + (addr & 0x03FF);
            }
            else if (addr <= 0x17FF) {
                mapped_addr = chr_banks[3] * 0x0400 + (addr & 0x03FF);
            }
            else if (addr <= 0x1BFF) {
                mapped_addr = chr_banks[4] * 0x0400 + (addr & 0x03FF);
            }
            else {
                mapped_addr = chr_banks[5] * 0x0400 + (addr & 0x03FF);
            }
        }

        if (mapped_addr < cartridge->vCHRMemory.size()) {
            data = cartridge->vCHRMemory[mapped_addr];
            return true;
        }
    }
    return false;
}

bool Mapper004::ppuWrite(uint16_t addr, uint8_t data) {

    return false;
}

// ФІКС: Правильна реалізація scanline для MMC3
void Mapper004::scanline() {
    // Обробка IRQ reload
    if (irq_reload) {
        irq_counter = irq_latch;
        irq_reload = false;
    }
    // Декремент лічильника, якщо він не нуль
    else if (irq_counter > 0) {
        irq_counter--;
    }

    // Активувати IRQ, коли лічильник досягає нуля і IRQ дозволено
    if (irq_counter == 0 && irq_enabled) {
        irq_active = true;
    }
}

// ФІКС: Додавання відсутніх методів IRQ
bool Mapper004::irqState() {
    return irq_active;
}

void Mapper004::irqClear() {
    irq_active = false;
}


Mapper005::Mapper005(Cartridge* cart) : Mapper(cart) {}

bool Mapper005::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        uint32_t mapped_addr = prgBank * 0x8000 + (addr & 0x7FFF);
        data = cartridge->vPRGMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper005::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        prgBank = data & 0x07;
        return true;
    }
    return false;
}

bool Mapper005::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        uint32_t bank = (addr < 0x1000) ? chrBank0 : chrBank1;
        uint32_t mapped_addr = bank * 0x1000 + (addr & 0x0FFF);
        data = cartridge->vCHRMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper005::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr <= 0x1FFF && cartridge->nCHRBanks == 0) {
        cartridge->vCHRMemory[addr] = data;
        return true;
    }
    return false;
}


Mapper006::Mapper006(Cartridge* cart) : Mapper(cart) {}

bool Mapper006::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        uint32_t mapped_addr = prgBank * 0x8000 + (addr & 0x7FFF);
        data = cartridge->vPRGMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper006::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        prgBank = data & 0x07;
        chrBank = (data >> 4) & 0x07;
        return true;
    }
    return false;
}

bool Mapper006::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        uint32_t mapped_addr = chrBank * 0x2000 + addr;
        data = cartridge->vCHRMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper006::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr <= 0x1FFF && cartridge->nCHRBanks == 0) {
        cartridge->vCHRMemory[addr] = data;
        return true;
    }
    return false;
}


Mapper007::Mapper007(Cartridge* cart) : Mapper(cart) {}

bool Mapper007::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        uint32_t mapped_addr = prgBank * 0x8000 + (addr & 0x7FFF);
        data = cartridge->vPRGMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper007::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        prgBank = data & 0x07;
        return true;
    }
    return false;
}

bool Mapper007::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        data = cartridge->vCHRMemory[addr];
        return true;
    }
    return false;
}

bool Mapper007::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr <= 0x1FFF && cartridge->nCHRBanks == 0) {
        cartridge->vCHRMemory[addr] = data;
        return true;
    }
    return false;
}


Mapper016::Mapper016(Cartridge* cart) : Mapper(cart) {}

bool Mapper016::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        uint32_t mapped_addr = prgBank * 0x8000 + (addr & 0x7FFF);
        data = cartridge->vPRGMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper016::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        prgBank = data & 0x0F;
        return true;
    }
    return false;
}

bool Mapper016::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        uint32_t bank = (addr < 0x1000) ? chrBank0 : chrBank1;
        uint32_t mapped_addr = bank * 0x1000 + (addr & 0x0FFF);
        data = cartridge->vCHRMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper016::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr <= 0x1FFF && cartridge->nCHRBanks == 0) {
        cartridge->vCHRMemory[addr] = data;
        return true;
    }
    return false;
}


Mapper018::Mapper018(Cartridge* cart) : Mapper(cart) {}

bool Mapper018::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        uint32_t mapped_addr = prgBank * 0x8000 + (addr & 0x7FFF);
        data = cartridge->vPRGMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper018::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        prgBank = data & 0x0F;
        if (addr >= 0xB000 && addr <= 0xE000) {
            if (addr < 0xC000) chrBank0 = data;
            else chrBank1 = data;
        }
        return true;
    }
    return false;
}

bool Mapper018::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        uint32_t bank = (addr < 0x1000) ? chrBank0 : chrBank1;
        uint32_t mapped_addr = bank * 0x1000 + (addr & 0x0FFF);
        data = cartridge->vCHRMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper018::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr <= 0x1FFF && cartridge->nCHRBanks == 0) {
        cartridge->vCHRMemory[addr] = data;
        return true;
    }
    return false;
}


Mapper019::Mapper019(Cartridge* cart) : Mapper(cart) {}

bool Mapper019::cpuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        uint32_t mapped_addr = prgBank * 0x8000 + (addr & 0x7FFF);
        data = cartridge->vPRGMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper019::cpuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        prgBank = data & 0x0F;
        if (addr >= 0xB000 && addr <= 0xE000) {
            if (addr < 0xC000) chrBank0 = data;
            else chrBank1 = data;
        }
        return true;
    }
    return false;
}

bool Mapper019::ppuRead(uint16_t addr, uint8_t& data) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        uint32_t bank = (addr < 0x1000) ? chrBank0 : chrBank1;
        uint32_t mapped_addr = bank * 0x1000 + (addr & 0x0FFF);
        data = cartridge->vCHRMemory[mapped_addr];
        return true;
    }
    return false;
}

bool Mapper019::ppuWrite(uint16_t addr, uint8_t data) {
    if (addr >= 0x0000 && addr <= 0x1FFF && cartridge->nCHRBanks == 0) {
        cartridge->vCHRMemory[addr] = data;
        return true;
    }
    return false;
}