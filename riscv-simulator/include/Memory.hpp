#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class Memory {
public:
    explicit Memory(std::size_t size = 16 * 1024 * 1024);

    bool loadBinary(const std::string& path, uint32_t startAddress = 0);
    uint8_t read8(uint32_t address) const;
    uint16_t read16(uint32_t address) const;
    uint32_t read32(uint32_t address) const;
    void write8(uint32_t address, uint8_t value);
    void write16(uint32_t address, uint16_t value);
    void write32(uint32_t address, uint32_t value);
    void dump(uint32_t start, uint32_t end) const;
    std::size_t size() const noexcept;

private:
    std::vector<uint8_t> bytes;
    void checkRange(uint32_t address, std::size_t count) const;
};
