#include "Simulator.hpp"
#include "Decoder.hpp"
#include <iomanip>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>

Simulator::Simulator(std::size_t memorySize) : memory(memorySize) {}

bool Simulator::loadProgram(const std::string& path) {
    reset();
    if (!memory.loadBinary(path, 0)) return false;

    // Determine file length for end-of-program detection.
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;
    loadedBytes = static_cast<uint64_t>(file.tellg());
    if (loadedBytes == 0) halted = true;
    return true;
}

uint32_t Simulator::reg(unsigned i) const {
    if (i >= 32) throw std::out_of_range("Indice de registro invalido.");
    return i == 0 ? 0 : regs[i];
}

void Simulator::setReg(unsigned i, uint32_t value) {
    if (i != 0) regs[i] = value;
}

int32_t Simulator::asSigned(uint32_t value) {
    return static_cast<int32_t>(value);
}

uint32_t Simulator::arithmeticRightShift(uint32_t value, unsigned amount) {
    amount &= 31u;
    if (amount == 0) return value;
    uint32_t result = value >> amount;
    if (value & 0x80000000u) result |= (~uint32_t{0} << (32 - amount));
    return result;
}

void Simulator::execute(uint32_t instruction) {
    const auto d = Decoder::decode(instruction);
    const uint32_t a = reg(d.rs1);
    const uint32_t b = reg(d.rs2);
    const uint32_t oldPC = pc;
    uint32_t nextPC = pc + 4;

    switch (d.opcode) {
        // Loads
        case 0x03: {
            const uint32_t address = a + static_cast<uint32_t>(d.immediate);
            switch (d.funct3) {
                case 0: setReg(d.rd, static_cast<uint32_t>(
                              static_cast<int32_t>(static_cast<int8_t>(memory.read8(address))))); break; // LB
                case 1: setReg(d.rd, static_cast<uint32_t>(
                              static_cast<int32_t>(static_cast<int16_t>(memory.read16(address))))); break; // LH
                case 2: setReg(d.rd, memory.read32(address)); break; // LW
                case 4: setReg(d.rd, memory.read8(address)); break; // LBU
                case 5: setReg(d.rd, memory.read16(address)); break; // LHU
                default: throw std::runtime_error("funct3 invalido para LOAD.");
            }
            break;
        }
        // Immediate arithmetic
        case 0x13:
            switch (d.funct3) {
                case 0: setReg(d.rd, a + static_cast<uint32_t>(d.immediate)); break; // ADDI
                case 1:
                    if (d.funct7 != 0) throw std::runtime_error("SLLI invalido.");
                    setReg(d.rd, a << (d.rs2 & 31)); break;
                case 2: setReg(d.rd, asSigned(a) < d.immediate ? 1u : 0u); break; // SLTI
                case 3: setReg(d.rd, a < static_cast<uint32_t>(d.immediate) ? 1u : 0u); break; // SLTIU
                case 4: setReg(d.rd, a ^ static_cast<uint32_t>(d.immediate)); break; // XORI
                case 5:
                    if (d.funct7 == 0) setReg(d.rd, a >> (d.rs2 & 31)); // SRLI
                    else if (d.funct7 == 0x20) setReg(d.rd, arithmeticRightShift(a, d.rs2)); // SRAI
                    else throw std::runtime_error("SRLI/SRAI invalido.");
                    break;
                case 6: setReg(d.rd, a | static_cast<uint32_t>(d.immediate)); break; // ORI
                case 7: setReg(d.rd, a & static_cast<uint32_t>(d.immediate)); break; // ANDI
                default: throw std::runtime_error("funct3 invalido para OP-IMM.");
            }
            break;
        // AUIPC
        case 0x17:
            setReg(d.rd, oldPC + static_cast<uint32_t>(d.immediate));
            break;
        // Stores
        case 0x23: {
            const uint32_t address = a + static_cast<uint32_t>(d.immediate);
            switch (d.funct3) {
                case 0: memory.write8(address, static_cast<uint8_t>(b)); break; // SB
                case 1: memory.write16(address, static_cast<uint16_t>(b)); break; // SH
                case 2: memory.write32(address, b); break; // SW
                default: throw std::runtime_error("funct3 invalido para STORE.");
            }
            break;
        }
        // Register-register arithmetic
        case 0x33:
            switch (d.funct3) {
                case 0:
                    if (d.funct7 == 0) setReg(d.rd, a + b); // ADD
                    else if (d.funct7 == 0x20) setReg(d.rd, a - b); // SUB
                    else throw std::runtime_error("ADD/SUB invalido.");
                    break;
                case 1: if (d.funct7 != 0) throw std::runtime_error("SLL invalido.");
                        setReg(d.rd, a << (b & 31)); break;
                case 2: if (d.funct7 != 0) throw std::runtime_error("SLT invalido.");
                        setReg(d.rd, asSigned(a) < asSigned(b) ? 1u : 0u); break;
                case 3: if (d.funct7 != 0) throw std::runtime_error("SLTU invalido.");
                        setReg(d.rd, a < b ? 1u : 0u); break;
                case 4: if (d.funct7 != 0) throw std::runtime_error("XOR invalido.");
                        setReg(d.rd, a ^ b); break;
                case 5:
                    if (d.funct7 == 0) setReg(d.rd, a >> (b & 31)); // SRL
                    else if (d.funct7 == 0x20) setReg(d.rd, arithmeticRightShift(a, b)); // SRA
                    else throw std::runtime_error("SRL/SRA invalido.");
                    break;
                case 6: if (d.funct7 != 0) throw std::runtime_error("OR invalido.");
                        setReg(d.rd, a | b); break;
                case 7: if (d.funct7 != 0) throw std::runtime_error("AND invalido.");
                        setReg(d.rd, a & b); break;
                default: throw std::runtime_error("funct3 invalido para OP.");
            }
            break;
        // LUI
        case 0x37:
            setReg(d.rd, static_cast<uint32_t>(d.immediate));
            break;
        // Branches
        case 0x63: {
            bool take = false;
            switch (d.funct3) {
                case 0: take = (a == b); break; // BEQ
                case 1: take = (a != b); break; // BNE
                case 4: take = (asSigned(a) < asSigned(b)); break; // BLT
                case 5: take = (asSigned(a) >= asSigned(b)); break; // BGE
                case 6: take = (a < b); break; // BLTU
                case 7: take = (a >= b); break; // BGEU
                default: throw std::runtime_error("funct3 invalido para BRANCH.");
            }
            if (take) nextPC = oldPC + static_cast<uint32_t>(d.immediate);
            break;
        }
        // JALR
        case 0x67:
            if (d.funct3 != 0) throw std::runtime_error("JALR invalido.");
            setReg(d.rd, oldPC + 4);
            nextPC = (a + static_cast<uint32_t>(d.immediate)) & ~uint32_t{1};
            break;
        // JAL
        case 0x6f:
            setReg(d.rd, oldPC + 4);
            nextPC = oldPC + static_cast<uint32_t>(d.immediate);
            break;
        default:
            throw std::runtime_error("Opcode no soportado: " + std::to_string(d.opcode));
    }

    pc = nextPC;
    regs[0] = 0;
}

bool Simulator::step() {
    if (halted) {
        std::cout << "El programa ya termino.\n";
        return false;
    }
    if (pc >= loadedBytes || loadedBytes - pc < 4) {
        halted = true;
        std::cout << "Fin del programa (no queda una instruccion completa en el binario).\n";
        return false;
    }
    if ((pc & 3u) != 0) throw std::runtime_error("PC no alineado a 4 bytes.");
    const uint32_t instruction = memory.read32(pc);
    std::cout << "PC=0x" << std::hex << std::setw(8) << std::setfill('0') << pc
              << "  instruccion=0x" << std::setw(8) << instruction
              << std::dec << std::setfill(' ') << '\n';
    execute(instruction);
    ++executed;
    if (pc >= loadedBytes) halted = true;
    return true;
}

void Simulator::run(uint64_t maxSteps) {
    uint64_t count = 0;
    while (!halted && count < maxSteps) {
        if (!step()) break;
        ++count;
    }
    if (!halted && count == maxSteps)
        std::cout << "Se alcanzo el limite de " << maxSteps << " instrucciones.\n";
    std::cout << "Instrucciones ejecutadas en esta corrida: " << count << '\n';
}

void Simulator::showPC() const {
    std::cout << "pc = 0x" << std::hex << std::setw(8) << std::setfill('0')
              << pc << std::dec << std::setfill(' ') << '\n';
}

void Simulator::showRegisters() const {
    for (unsigned i = 0; i < 32; ++i) showRegister(i);
}

void Simulator::showRegister(unsigned i) const {
    if (i >= 32) throw std::out_of_range("Registro debe estar entre x0 y x31.");
    std::cout << 'x' << i << " = 0x" << std::hex << std::setw(8)
              << std::setfill('0') << reg(i) << std::dec << std::setfill(' ')
              << '\n';
}

void Simulator::showMemory(uint32_t start, uint32_t end) const {
    memory.dump(start, end);
}

void Simulator::reset() {
    pc = 0;
    regs.fill(0);
    halted = false;
    executed = 0;
}

bool Simulator::isHalted() const noexcept { return halted; }
uint32_t Simulator::getPC() const noexcept { return pc; }
