#include "Cpu.h"
#include "Bus.h"

Cpu::Cpu() {
    using a = Cpu;
    lookup = {
        { "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "NOP", &a::NOP, &a::ZP0, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "XXX", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "NOP", &a::NOP, &a::ABS, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },
        { "BPL", &a::BPL, &a::REL, 2 },{ "ORA", &a::ORA, &a::IZY, 5 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "NOP", &a::NOP, &a::ZPX, 4 },{ "ORA", &a::ORA, &a::ZPX, 4 },{ "ASL", &a::ASL, &a::ZPX, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },{ "CLC", &a::CLC, &a::IMP, 2 },{ "ORA", &a::ORA, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 7 },{ "NOP", &a::NOP, &a::ABX, 4 },{ "ORA", &a::ORA, &a::ABX, 4 },{ "ASL", &a::ASL, &a::ABX, 7 },{ "XXX", &a::XXX, &a::IMP, 7 },
        { "JSR", &a::JSR, &a::ABS, 6 },{ "AND", &a::AND, &a::IZX, 6 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "BIT", &a::BIT, &a::ZP0, 3 },{ "AND", &a::AND, &a::ZP0, 3 },{ "ROL", &a::ROL, &a::ZP0, 5 },{ "XXX", &a::XXX, &a::IMP, 5 },{ "PLP", &a::PLP, &a::IMP, 4 },{ "AND", &a::AND, &a::IMM, 2 },{ "ROL", &a::ROL, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "BIT", &a::BIT, &a::ABS, 4 },{ "AND", &a::AND, &a::ABS, 4 },{ "ROL", &a::ROL, &a::ABS, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },
        { "BMI", &a::BMI, &a::REL, 2 },{ "AND", &a::AND, &a::IZY, 5 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "NOP", &a::NOP, &a::ZPX, 4 },{ "AND", &a::AND, &a::ZPX, 4 },{ "ROL", &a::ROL, &a::ZPX, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },{ "SEC", &a::SEC, &a::IMP, 2 },{ "AND", &a::AND, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 7 },{ "NOP", &a::NOP, &a::ABX, 4 },{ "AND", &a::AND, &a::ABX, 4 },{ "ROL", &a::ROL, &a::ABX, 7 },{ "XXX", &a::XXX, &a::IMP, 7 },
        { "RTI", &a::RTI, &a::IMP, 6 },{ "EOR", &a::EOR, &a::IZX, 6 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "NOP", &a::NOP, &a::ZP0, 3 },{ "EOR", &a::EOR, &a::ZP0, 3 },{ "LSR", &a::LSR, &a::ZP0, 5 },{ "XXX", &a::XXX, &a::IMP, 5 },{ "PHA", &a::PHA, &a::IMP, 3 },{ "EOR", &a::EOR, &a::IMM, 2 },{ "LSR", &a::LSR, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::ABS, 3 },{ "EOR", &a::EOR, &a::ABS, 4 },{ "LSR", &a::LSR, &a::ABS, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },
        { "BVC", &a::BVC, &a::REL, 2 },{ "EOR", &a::EOR, &a::IZY, 5 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "NOP", &a::NOP, &a::ZPX, 4 },{ "EOR", &a::EOR, &a::ZPX, 4 },{ "LSR", &a::LSR, &a::ZPX, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },{ "CLI", &a::CLI, &a::IMP, 2 },{ "EOR", &a::EOR, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 7 },{ "NOP", &a::NOP, &a::ABX, 4 },{ "EOR", &a::EOR, &a::ABX, 4 },{ "LSR", &a::LSR, &a::ABX, 7 },{ "XXX", &a::XXX, &a::IMP, 7 },
        { "RTS", &a::RTS, &a::IMP, 6 },{ "ADC", &a::ADC, &a::IZX, 6 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "NOP", &a::NOP, &a::ZP0, 3 },{ "ADC", &a::ADC, &a::ZP0, 3 },{ "ROR", &a::ROR, &a::ZP0, 5 },{ "XXX", &a::XXX, &a::IMP, 5 },{ "PLA", &a::PLA, &a::IMP, 4 },{ "ADC", &a::ADC, &a::IMM, 2 },{ "ROR", &a::ROR, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::IND, 5 },{ "ADC", &a::ADC, &a::ABS, 4 },{ "ROR", &a::ROR, &a::ABS, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },
        { "BVS", &a::BVS, &a::REL, 2 },{ "ADC", &a::ADC, &a::IZY, 5 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "NOP", &a::NOP, &a::ZPX, 4 },{ "ADC", &a::ADC, &a::ZPX, 4 },{ "ROR", &a::ROR, &a::ZPX, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },{ "SEI", &a::SEI, &a::IMP, 2 },{ "ADC", &a::ADC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 7 },{ "NOP", &a::NOP, &a::ABX, 4 },{ "ADC", &a::ADC, &a::ABX, 4 },{ "ROR", &a::ROR, &a::ABX, 7 },{ "XXX", &a::XXX, &a::IMP, 7 },
        { "NOP", &a::NOP, &a::IMM, 2 },{ "STA", &a::STA, &a::IZX, 6 },{ "NOP", &a::NOP, &a::IMM, 2 },{ "XXX", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZP0, 3 },{ "STA", &a::STA, &a::ZP0, 3 },{ "STX", &a::STX, &a::ZP0, 3 },{ "XXX", &a::XXX, &a::IMP, 3 },{ "DEY", &a::DEY, &a::IMP, 2 },{ "NOP", &a::NOP, &a::IMM, 2 },{ "TXA", &a::TXA, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "STY", &a::STY, &a::ABS, 4 },{ "STA", &a::STA, &a::ABS, 4 },{ "STX", &a::STX, &a::ABS, 4 },{ "XXX", &a::XXX, &a::IMP, 4 },
        { "BCC", &a::BCC, &a::REL, 2 },{ "STA", &a::STA, &a::IZY, 6 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZPX, 4 },{ "STA", &a::STA, &a::ZPX, 4 },{ "STX", &a::STX, &a::ZPY, 4 },{ "XXX", &a::XXX, &a::IMP, 4 },{ "TYA", &a::TYA, &a::IMP, 2 },{ "STA", &a::STA, &a::ABY, 5 },{ "TXS", &a::TXS, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 5 },{ "NOP", &a::NOP, &a::ABX, 5 },{ "STA", &a::STA, &a::ABX, 5 },{ "XXX", &a::XXX, &a::ABY, 5 },{ "XXX", &a::XXX, &a::IMP, 5 },
        { "LDY", &a::LDY, &a::IMM, 2 },{ "LDA", &a::LDA, &a::IZX, 6 },{ "LDX", &a::LDX, &a::IMM, 2 },{ "XXX", &a::XXX, &a::IMP, 6 },{ "LDY", &a::LDY, &a::ZP0, 3 },{ "LDA", &a::LDA, &a::ZP0, 3 },{ "LDX", &a::LDX, &a::ZP0, 3 },{ "XXX", &a::XXX, &a::IMP, 3 },{ "TAY", &a::TAY, &a::IMP, 2 },{ "LDA", &a::LDA, &a::IMM, 2 },{ "TAX", &a::TAX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "LDY", &a::LDY, &a::ABS, 4 },{ "LDA", &a::LDA, &a::ABS, 4 },{ "LDX", &a::LDX, &a::ABS, 4 },{ "XXX", &a::XXX, &a::IMP, 4 },
        { "BCS", &a::BCS, &a::REL, 2 },{ "LDA", &a::LDA, &a::IZY, 5 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 5 },{ "LDY", &a::LDY, &a::ZPX, 4 },{ "LDA", &a::LDA, &a::ZPX, 4 },{ "LDX", &a::LDX, &a::ZPY, 4 },{ "XXX", &a::XXX, &a::IMP, 4 },{ "CLV", &a::CLV, &a::IMP, 2 },{ "LDA", &a::LDA, &a::ABY, 4 },{ "TSX", &a::TSX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 4 },{ "LDY", &a::LDY, &a::ABX, 4 },{ "LDA", &a::LDA, &a::ABX, 4 },{ "LDX", &a::LDX, &a::ABY, 4 },{ "XXX", &a::XXX, &a::IMP, 4 },
        { "CPY", &a::CPY, &a::IMM, 2 },{ "CMP", &a::CMP, &a::IZX, 6 },{ "NOP", &a::NOP, &a::IMM, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "CPY", &a::CPY, &a::ZP0, 3 },{ "CMP", &a::CMP, &a::ZP0, 3 },{ "DEC", &a::DEC, &a::ZP0, 5 },{ "XXX", &a::XXX, &a::IMP, 5 },{ "INY", &a::INY, &a::IMP, 2 },{ "CMP", &a::CMP, &a::IMM, 2 },{ "DEX", &a::DEX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "CPY", &a::CPY, &a::ABS, 4 },{ "CMP", &a::CMP, &a::ABS, 4 },{ "DEC", &a::DEC, &a::ABS, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },
        { "BNE", &a::BNE, &a::REL, 2 },{ "CMP", &a::CMP, &a::IZY, 5 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "NOP", &a::NOP, &a::ZPX, 4 },{ "CMP", &a::CMP, &a::ZPX, 4 },{ "DEC", &a::DEC, &a::ZPX, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },{ "CLD", &a::CLD, &a::IMP, 2 },{ "CMP", &a::CMP, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 7 },{ "NOP", &a::NOP, &a::ABX, 4 },{ "CMP", &a::CMP, &a::ABX, 4 },{ "DEC", &a::DEC, &a::ABX, 7 },{ "XXX", &a::XXX, &a::IMP, 7 },
        { "CPX", &a::CPX, &a::IMM, 2 },{ "SBC", &a::SBC, &a::IZX, 6 },{ "NOP", &a::NOP, &a::IMM, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "CPX", &a::CPX, &a::ZP0, 3 },{ "SBC", &a::SBC, &a::ZP0, 3 },{ "INC", &a::INC, &a::ZP0, 5 },{ "XXX", &a::XXX, &a::IMP, 5 },{ "INX", &a::INX, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMM, 2 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMP, 2 },{ "CPX", &a::CPX, &a::ABS, 4 },{ "SBC", &a::SBC, &a::ABS, 4 },{ "INC", &a::INC, &a::ABS, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },
        { "BEQ", &a::BEQ, &a::REL, 2 },{ "SBC", &a::SBC, &a::IZY, 5 },{ "XXX", &a::XXX, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 8 },{ "NOP", &a::NOP, &a::ZPX, 4 },{ "SBC", &a::SBC, &a::ZPX, 4 },{ "INC", &a::INC, &a::ZPX, 6 },{ "XXX", &a::XXX, &a::IMP, 6 },{ "SED", &a::SED, &a::IMP, 2 },{ "SBC", &a::SBC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "XXX", &a::XXX, &a::IMP, 7 },{ "NOP", &a::NOP, &a::ABX, 4 },{ "SBC", &a::SBC, &a::ABX, 4 },{ "INC", &a::INC, &a::ABX, 7 },{ "XXX", &a::XXX, &a::IMP, 7 },
    };
}

Cpu::~Cpu() {}

uint8_t Cpu::getFlag(FLAGS6502 f) {
    return ((status & f) > 0) ? 1 : 0;
}

void Cpu::setFlag(FLAGS6502 f, bool v) {
    if (v) status |= f;
    else status &= ~f;
}

void Cpu::clock() {
    if (cycles == 0) {
        opcode = bus->cpuRead(pc, true);
        setFlag(U, true);
        pc++;
        cycles = lookup[opcode].cycles;
        uint8_t additional_cycle1 = (this->*lookup[opcode].addrmode)();
        uint8_t additional_cycle2 = (this->*lookup[opcode].operate)();
        cycles += (additional_cycle1 & additional_cycle2);
        setFlag(U, true);
    }
    cycles--;
}

void Cpu::reset() {
    a = 0; x = 0; y = 0; stkp = 0xFD; status = 0x00 | U;
    addr_abs = 0xFFFC;
    uint16_t lo = bus->cpuRead(addr_abs + 0, true);
    uint16_t hi = bus->cpuRead(addr_abs + 1, true);
    pc = (hi << 8) | lo;
    addr_rel = 0x0000; addr_abs = 0x0000; fetched = 0x00;
    cycles = 8;
}

void Cpu::irq() {
    if (getFlag(I) == 0) {
        bus->cpuWrite(0x0100 + stkp, (pc >> 8) & 0x00FF); stkp--;
        bus->cpuWrite(0x0100 + stkp, pc & 0x00FF); stkp--;
        setFlag(B, 0); setFlag(U, 1); setFlag(I, 1);
        bus->cpuWrite(0x0100 + stkp, status); stkp--;
        addr_abs = 0xFFFE;
        uint16_t lo = bus->cpuRead(addr_abs + 0, true);
        uint16_t hi = bus->cpuRead(addr_abs + 1, true);
        pc = (hi << 8) | lo;
        cycles = 7;
    }
}

void Cpu::nmi() {
    bus->cpuWrite(0x0100 + stkp, (pc >> 8) & 0x00FF); stkp--;
    bus->cpuWrite(0x0100 + stkp, pc & 0x00FF); stkp--;
    setFlag(B, 0); setFlag(U, 1); setFlag(I, 1);
    bus->cpuWrite(0x0100 + stkp, status); stkp--;
    addr_abs = 0xFFFA;
    uint16_t lo = bus->cpuRead(addr_abs + 0, true);
    uint16_t hi = bus->cpuRead(addr_abs + 1, true);
    pc = (hi << 8) | lo;
    cycles = 8;
}

uint8_t Cpu::fetch() {
    if (!(lookup[opcode].addrmode == &Cpu::IMP))
        fetched = bus->cpuRead(addr_abs, false);
    return fetched;
}

uint8_t Cpu::IMP() { fetched = a; return 0; }
uint8_t Cpu::IMM() { addr_abs = pc++; return 0; }
uint8_t Cpu::ZP0() { addr_abs = bus->cpuRead(pc, true); pc++; addr_abs &= 0x00FF; return 0; }
uint8_t Cpu::ZPX() { addr_abs = (bus->cpuRead(pc, true) + x); pc++; addr_abs &= 0x00FF; return 0; }
uint8_t Cpu::ZPY() { addr_abs = (bus->cpuRead(pc, true) + y); pc++; addr_abs &= 0x00FF; return 0; }
uint8_t Cpu::REL() { addr_rel = bus->cpuRead(pc, true); pc++; if (addr_rel & 0x80) addr_rel |= 0xFF00; return 0; }
uint8_t Cpu::ABS() { uint16_t lo = bus->cpuRead(pc, true); pc++; uint16_t hi = bus->cpuRead(pc, true); pc++; addr_abs = (hi << 8) | lo; return 0; }
uint8_t Cpu::ABX() { uint16_t lo = bus->cpuRead(pc, true); pc++; uint16_t hi = bus->cpuRead(pc, true); pc++; addr_abs = ((hi << 8) | lo) + x; return ((addr_abs & 0xFF00) != (hi << 8)); }
uint8_t Cpu::ABY() { uint16_t lo = bus->cpuRead(pc, true); pc++; uint16_t hi = bus->cpuRead(pc, true); pc++; addr_abs = ((hi << 8) | lo) + y; return ((addr_abs & 0xFF00) != (hi << 8)); }
uint8_t Cpu::IND() { uint16_t ptr_lo = bus->cpuRead(pc, true); pc++; uint16_t ptr_hi = bus->cpuRead(pc, true); pc++; uint16_t ptr = (ptr_hi << 8) | ptr_lo; if (ptr_lo == 0x00FF) addr_abs = (bus->cpuRead(ptr & 0xFF00, true) << 8) | bus->cpuRead(ptr + 0, true); else addr_abs = (bus->cpuRead(ptr + 1, true) << 8) | bus->cpuRead(ptr + 0, true); return 0; }
uint8_t Cpu::IZX() { uint16_t t = bus->cpuRead(pc, true); pc++; uint16_t lo = bus->cpuRead((uint16_t)(t + x) & 0x00FF, true); uint16_t hi = bus->cpuRead((uint16_t)(t + x + 1) & 0x00FF, true); addr_abs = (hi << 8) | lo; return 0; }
uint8_t Cpu::IZY() { uint16_t t = bus->cpuRead(pc, true); pc++; uint16_t lo = bus->cpuRead(t & 0x00FF, true); uint16_t hi = bus->cpuRead((t + 1) & 0x00FF, true); addr_abs = ((hi << 8) | lo) + y; return ((addr_abs & 0xFF00) != (hi << 8)); }
uint8_t Cpu::ADC() { fetch(); uint16_t temp = (uint16_t)a + (uint16_t)fetched + (uint16_t)getFlag(C); setFlag(C, temp > 255); setFlag(Z, (temp & 0x00FF) == 0); setFlag(V, (~((uint16_t)a ^ (uint16_t)fetched) & ((uint16_t)a ^ (uint16_t)temp)) & 0x0080); setFlag(N, temp & 0x80); a = temp & 0x00FF; return 1; }
uint8_t Cpu::AND() { fetch(); a = a & fetched; setFlag(Z, a == 0x00); setFlag(N, a & 0x80); return 1; }
uint8_t Cpu::ASL() { fetch(); uint16_t temp = (uint16_t)fetched << 1; setFlag(C, (temp & 0xFF00) > 0); setFlag(Z, (temp & 0x00FF) == 0x00); setFlag(N, temp & 0x80); if (lookup[opcode].addrmode == &Cpu::IMP) a = temp & 0x00FF; else bus->cpuWrite(addr_abs, temp & 0x00FF); return 0; }
uint8_t Cpu::BCC() { if (getFlag(C) == 0) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t Cpu::BCS() { if (getFlag(C) == 1) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t Cpu::BEQ() { if (getFlag(Z) == 1) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t Cpu::BIT() { fetch(); uint16_t temp = a & fetched; setFlag(Z, (temp & 0x00FF) == 0x00); setFlag(N, fetched & (1 << 7)); setFlag(V, fetched & (1 << 6)); return 0; }
uint8_t Cpu::BMI() { if (getFlag(N) == 1) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t Cpu::BNE() { if (getFlag(Z) == 0) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t Cpu::BPL() { if (getFlag(N) == 0) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t Cpu::BRK() { pc++; setFlag(I, 1); bus->cpuWrite(0x0100 + stkp, (pc >> 8) & 0x00FF); stkp--; bus->cpuWrite(0x0100 + stkp, pc & 0x00FF); stkp--; setFlag(B, 1); bus->cpuWrite(0x0100 + stkp, status); stkp--; setFlag(B, 0); pc = (uint16_t)bus->cpuRead(0xFFFE, true) | ((uint16_t)bus->cpuRead(0xFFFF, true) << 8); return 0; }
uint8_t Cpu::BVC() { if (getFlag(V) == 0) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t Cpu::BVS() { if (getFlag(V) == 1) { cycles++; addr_abs = pc + addr_rel; if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++; pc = addr_abs; } return 0; }
uint8_t Cpu::CLC() { setFlag(C, false); return 0; }
uint8_t Cpu::CLD() { setFlag(D, false); return 0; }
uint8_t Cpu::CLI() { setFlag(I, false); return 0; }
uint8_t Cpu::CLV() { setFlag(V, false); return 0; }
uint8_t Cpu::CMP() { fetch(); uint16_t temp = (uint16_t)a - (uint16_t)fetched; setFlag(C, a >= fetched); setFlag(Z, (temp & 0x00FF) == 0x0000); setFlag(N, temp & 0x0080); return 1; }
uint8_t Cpu::CPX() { fetch(); uint16_t temp = (uint16_t)x - (uint16_t)fetched; setFlag(C, x >= fetched); setFlag(Z, (temp & 0x00FF) == 0x0000); setFlag(N, temp & 0x0080); return 0; }
uint8_t Cpu::CPY() { fetch(); uint16_t temp = (uint16_t)y - (uint16_t)fetched; setFlag(C, y >= fetched); setFlag(Z, (temp & 0x00FF) == 0x0000); setFlag(N, temp & 0x0080); return 0; }
uint8_t Cpu::DEC() { fetch(); uint16_t temp = fetched - 1; bus->cpuWrite(addr_abs, temp & 0x00FF); setFlag(Z, (temp & 0x00FF) == 0x0000); setFlag(N, temp & 0x0080); return 0; }
uint8_t Cpu::DEX() { x--; setFlag(Z, x == 0x00); setFlag(N, x & 0x80); return 0; }
uint8_t Cpu::DEY() { y--; setFlag(Z, y == 0x00); setFlag(N, y & 0x80); return 0; }
uint8_t Cpu::EOR() { fetch(); a = a ^ fetched; setFlag(Z, a == 0x00); setFlag(N, a & 0x80); return 1; }
uint8_t Cpu::INC() { fetch(); uint16_t temp = fetched + 1; bus->cpuWrite(addr_abs, temp & 0x00FF); setFlag(Z, (temp & 0x00FF) == 0x0000); setFlag(N, temp & 0x0080); return 0; }
uint8_t Cpu::INX() { x++; setFlag(Z, x == 0x00); setFlag(N, x & 0x80); return 0; }
uint8_t Cpu::INY() { y++; setFlag(Z, y == 0x00); setFlag(N, y & 0x80); return 0; }
uint8_t Cpu::JMP() { pc = addr_abs; return 0; }
uint8_t Cpu::JSR() { pc--; bus->cpuWrite(0x0100 + stkp, (pc >> 8) & 0x00FF); stkp--; bus->cpuWrite(0x0100 + stkp, pc & 0x00FF); stkp--; pc = addr_abs; return 0; }
uint8_t Cpu::LDA() { fetch(); a = fetched; setFlag(Z, a == 0x00); setFlag(N, a & 0x80); return 1; }
uint8_t Cpu::LDX() { fetch(); x = fetched; setFlag(Z, x == 0x00); setFlag(N, x & 0x80); return 1; }
uint8_t Cpu::LDY() { fetch(); y = fetched; setFlag(Z, y == 0x00); setFlag(N, y & 0x80); return 1; }
uint8_t Cpu::LSR() { fetch(); setFlag(C, fetched & 0x0001); uint16_t temp = fetched >> 1; setFlag(Z, (temp & 0x00FF) == 0x0000); setFlag(N, 0 > 0); if (lookup[opcode].addrmode == &Cpu::IMP) a = temp & 0x00FF; else bus->cpuWrite(addr_abs, temp & 0x00FF); return 0; }
uint8_t Cpu::NOP() { switch (opcode) { case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC: return 1; break; } return 0; }
                                                uint8_t Cpu::ORA() { fetch(); a = a | fetched; setFlag(Z, a == 0x00); setFlag(N, a & 0x80); return 1; }
                                                uint8_t Cpu::PHA() { bus->cpuWrite(0x0100 + stkp, a); stkp--; return 0; }
                                                uint8_t Cpu::PHP() { bus->cpuWrite(0x0100 + stkp, status | B | U); setFlag(B, 0); setFlag(U, 0); stkp--; return 0; }
                                                uint8_t Cpu::PLA() { stkp++; a = bus->cpuRead(0x0100 + stkp, true); setFlag(Z, a == 0x00); setFlag(N, a & 0x80); return 0; }
                                                uint8_t Cpu::PLP() { stkp++; status = bus->cpuRead(0x0100 + stkp, true); setFlag(U, 1); return 0; }
                                                uint8_t Cpu::ROL() { fetch(); uint16_t temp = (uint16_t)(fetched << 1) | getFlag(C); setFlag(C, temp & 0xFF00); setFlag(Z, (temp & 0x00FF) == 0x0000); setFlag(N, temp & 0x0080); if (lookup[opcode].addrmode == &Cpu::IMP) a = temp & 0x00FF; else bus->cpuWrite(addr_abs, temp & 0x00FF); return 0; }
                                                uint8_t Cpu::ROR() { fetch(); uint16_t temp = (uint16_t)(getFlag(C) << 7) | (fetched >> 1); setFlag(C, fetched & 0x01); setFlag(Z, (temp & 0x00FF) == 0x00); setFlag(N, temp & 0x0080); if (lookup[opcode].addrmode == &Cpu::IMP) a = temp & 0x00FF; else bus->cpuWrite(addr_abs, temp & 0x00FF); return 0; }
                                                uint8_t Cpu::RTI() { stkp++; status = bus->cpuRead(0x0100 + stkp, true); status &= ~B; status &= ~U; stkp++; pc = (uint16_t)bus->cpuRead(0x0100 + stkp, true); stkp++; pc |= (uint16_t)bus->cpuRead(0x0100 + stkp, true) << 8; return 0; }
                                                uint8_t Cpu::RTS() { stkp++; pc = (uint16_t)bus->cpuRead(0x0100 + stkp, true); stkp++; pc |= (uint16_t)bus->cpuRead(0x0100 + stkp, true) << 8; pc++; return 0; }
                                                uint8_t Cpu::SBC() { fetch(); uint16_t value = ((uint16_t)fetched) ^ 0x00FF; uint16_t temp = (uint16_t)a + value + (uint16_t)getFlag(C); setFlag(C, temp & 0xFF00); setFlag(Z, ((temp & 0x00FF) == 0)); setFlag(V, (temp ^ (uint16_t)a) & (temp ^ value) & 0x0080); setFlag(N, temp & 0x0080); a = temp & 0x00FF; return 1; }
                                                uint8_t Cpu::SEC() { setFlag(C, true); return 0; }
                                                uint8_t Cpu::SED() { setFlag(D, true); return 0; }
                                                uint8_t Cpu::SEI() { setFlag(I, true); return 0; }
                                                uint8_t Cpu::STA() { bus->cpuWrite(addr_abs, a); return 0; }
                                                uint8_t Cpu::STX() { bus->cpuWrite(addr_abs, x); return 0; }
                                                uint8_t Cpu::STY() { bus->cpuWrite(addr_abs, y); return 0; }
                                                uint8_t Cpu::TAX() { x = a; setFlag(Z, x == 0x00); setFlag(N, x & 0x80); return 0; }
                                                uint8_t Cpu::TAY() { y = a; setFlag(Z, y == 0x00); setFlag(N, y & 0x80); return 0; }
                                                uint8_t Cpu::TSX() { x = stkp; setFlag(Z, x == 0x00); setFlag(N, x & 0x80); return 0; }
                                                uint8_t Cpu::TXA() { a = x; setFlag(Z, a == 0x00); setFlag(N, a & 0x80); return 0; }
                                                uint8_t Cpu::TXS() { stkp = x; return 0; }
                                                uint8_t Cpu::TYA() { a = y; setFlag(Z, a == 0x00); setFlag(N, a & 0x80); return 0; }
                                                uint8_t Cpu::XXX() { return 0; }