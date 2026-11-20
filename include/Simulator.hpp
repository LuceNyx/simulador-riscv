#pragma once
#include "Memory.hpp"
#include <array>
#include <cstdint>
#include <string>

class Simulator {
public:
    explicit Simulator(std::size_t memorySize = 16 * 1024 * 1024);

    bool loadProgram(const std::string& path);
    bool step();
    void run(uint64_t maxSteps = 1000000);
    void showPC() const;
    void showRegisters() const;
    void showRegister(unsigned index) const;
    void showMemory(uint32_t start, uint32_t end) const;
    void showMemoryWords(uint32_t start, uint32_t count) const;
    void reset();
    bool isHalted() const noexcept;
    uint32_t getPC() const noexcept;

private:
    uint32_t pc = 0;
    std::array<uint32_t, 32> regs{};
    Memory memory;
    bool halted = false;
    uint64_t executed = 0;
    uint64_t loadedBytes = 0;

    uint32_t reg(unsigned index) const;
    void setReg(unsigned index, uint32_t value);
    void execute(uint32_t instruction);
    static int32_t asSigned(uint32_t value);
    static uint32_t arithmeticRightShift(uint32_t value, unsigned amount);
};
