#pragma once
#include <cstdint>
#include <vector>
#include <memory>

class Cartridge;

class Mapper {
public:
    Mapper(Cartridge* cart) : cartridge(cart) {}
    virtual ~Mapper() = default;

    virtual bool cpuRead(uint16_t addr, uint8_t& data) = 0;
    virtual bool cpuWrite(uint16_t addr, uint8_t data) = 0;
    virtual bool ppuRead(uint16_t addr, uint8_t& data) = 0;
    virtual bool ppuWrite(uint16_t addr, uint8_t data) = 0;


    virtual void scanline() {}
    virtual bool irqState() { return false; }
    virtual void irqClear() {}

    static std::unique_ptr<Mapper> createMapper(uint8_t mapperId, Cartridge* cart);

protected:
    Cartridge* cartridge;
};

class Mapper000 : public Mapper {
public:
    Mapper000(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;
};

class Mapper001 : public Mapper {
public:
    Mapper001(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;

private:
    uint8_t shiftRegister = 0x10;
    uint8_t control = 0x0C;
    uint8_t prgBank = 0;
    uint8_t chrBank0 = 0;
    uint8_t chrBank1 = 0;

    void writeRegister(uint16_t addr, uint8_t data);
};

class Mapper002 : public Mapper {
public:
    Mapper002(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;

private:
    uint8_t prgBank = 0;
};

class Mapper003 : public Mapper {
public:
    Mapper003(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;

private:
    uint8_t chrBank = 0;
};

class Mapper004 : public Mapper {
public:
    Mapper004(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;

    void scanline() override;
    bool irqState() override;
    void irqClear() override;

private:
    uint8_t target_register = 0;
    bool prg_bank_mode = false;
    bool chr_inversion = false;

    uint8_t prg_banks[2];
    uint8_t chr_banks[8];

    uint8_t irq_counter = 0;
    uint8_t irq_latch = 0;
    bool irq_enabled = false;
    bool irq_active = false;
    bool irq_reload = false;
};

class Mapper005 : public Mapper {
public:
    Mapper005(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;

private:
    uint8_t prgBank = 0;
    uint8_t chrBank0 = 0;
    uint8_t chrBank1 = 0;
    uint8_t mirroring = 0;
};

class Mapper006 : public Mapper {
public:
    Mapper006(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;

private:
    uint8_t prgBank = 0;
    uint8_t chrBank = 0;
};

class Mapper007 : public Mapper {
public:
    Mapper007(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;

private:
    uint8_t prgBank = 0;
};


class Mapper016 : public Mapper {
public:
    Mapper016(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;

private:
    uint8_t prgBank = 0;
    uint8_t chrBank0 = 0;
    uint8_t chrBank1 = 0;
};

class Mapper018 : public Mapper {
public:
    Mapper018(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;

private:
    uint8_t prgBank = 0;
    uint8_t chrBank0 = 0;
    uint8_t chrBank1 = 0;
    uint8_t irqCounter = 0;
    bool irqEnabled = false;
};

class Mapper019 : public Mapper {
public:
    Mapper019(Cartridge* cart);
    bool cpuRead(uint16_t addr, uint8_t& data) override;
    bool cpuWrite(uint16_t addr, uint8_t data) override;
    bool ppuRead(uint16_t addr, uint8_t& data) override;
    bool ppuWrite(uint16_t addr, uint8_t data) override;

private:
    uint8_t prgBank = 0;
    uint8_t chrBank0 = 0;
    uint8_t chrBank1 = 0;
    uint8_t irqCounter = 0;
    bool irqEnabled = false;
};