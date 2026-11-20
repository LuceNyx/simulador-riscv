#include "Simulator.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

class TemporaryProgram {
public:
    explicit TemporaryProgram(const std::string& name)
        : path_(std::filesystem::temp_directory_path() / name) {}

    ~TemporaryProgram() {
        std::error_code ignored;
        std::filesystem::remove(path_, ignored);
    }

    void write(const std::array<uint32_t, 2>& instructions, std::size_t count) const {
        std::ofstream file(path_, std::ios::binary | std::ios::trunc);
        if (!file) throw std::runtime_error("No se pudo crear el programa de prueba.");
        for (std::size_t i = 0; i < count; ++i) {
            const uint32_t word = instructions[i];
            for (unsigned byte = 0; byte < 4; ++byte)
                file.put(static_cast<char>((word >> (byte * 8)) & 0xff));
        }
        if (!file) throw std::runtime_error("No se pudo escribir el programa de prueba.");
    }

    const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void ebreakStopsOnItsAddress() {
    TemporaryProgram program("riscv-simulator-ebreak-test.bin");
    program.write({0x00100073u, 0u}, 1);
    Simulator simulator;
    require(simulator.loadProgram(program.path().string()), "No se pudo cargar EBREAK.");
    require(simulator.step(), "EBREAK debe contar como una instruccion ejecutada.");
    require(simulator.isHalted(), "EBREAK debe detener el simulador.");
    require(simulator.getPC() == 0, "El PC debe permanecer en la direccion de EBREAK.");
    require(!simulator.step(), "El simulador detenido no debe avanzar otra instruccion.");
}

void ebreakStopsAfterEarlierInstructions() {
    TemporaryProgram program("riscv-simulator-ebreak-after-addi-test.bin");
    program.write({0x02a00093u, 0x00100073u}, 2); // addi x1, x0, 42; ebreak
    Simulator simulator;
    require(simulator.loadProgram(program.path().string()), "No se pudo cargar el programa.");
    require(simulator.step(), "ADDI debe ejecutarse.");
    require(simulator.getPC() == 4, "El PC debe avanzar tras ADDI.");
    require(simulator.step(), "EBREAK debe ejecutarse.");
    require(simulator.isHalted(), "EBREAK debe detener el simulador.");
    require(simulator.getPC() == 4, "El PC debe apuntar a EBREAK.");
}

void otherSystemInstructionsRemainUnsupported() {
    TemporaryProgram program("riscv-simulator-system-test.bin");
    program.write({0x00000073u, 0u}, 1); // ECALL is not implemented.
    Simulator simulator;
    require(simulator.loadProgram(program.path().string()), "No se pudo cargar ECALL.");
    bool rejected = false;
    try {
        simulator.step();
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    require(rejected, "ECALL debe seguir reportandose como no soportado.");
    require(!simulator.isHalted(), "Una instruccion SYSTEM invalida no debe simular EBREAK.");
    require(simulator.getPC() == 0, "El PC no debe avanzar tras una instruccion invalida.");
}

std::string runExample(const std::string& relativePath, uint64_t maxSteps = 100000) {
    const auto path = std::filesystem::path(SIMULATOR_SOURCE_DIR) / relativePath;
    Simulator simulator;
    require(simulator.loadProgram(path.string()), "No se pudo cargar " + relativePath + ".");

    std::ostringstream output;
    auto* previous = std::cout.rdbuf(output.rdbuf());
    try {
        uint64_t steps = 0;
        while (!simulator.isHalted() && steps < maxSteps) {
            require(simulator.step(), "La ejecucion se detuvo antes de lo esperado.");
            ++steps;
        }
        require(simulator.isHalted(), "El programa no termino dentro del limite de pasos.");
        simulator.showMemory(0x64, 0x67);
        simulator.showMemory(0x1000, 0x101b);
        simulator.showRegister(10);
    } catch (...) {
        std::cout.rdbuf(previous);
        throw;
    }
    std::cout.rdbuf(previous);
    return output.str();
}

void referenceExampleProgramsProduceExpectedResults() {
    const auto riscvtest = runExample("programas/riscvtest.bin");
    require(riscvtest.find("19 00 00 00") != std::string::npos,
            "riscvtest debe dejar 25 en la direccion 0x64.");

    const auto symmetricTree = runExample("programas/symmetric-tree.bin");
    require(symmetricTree.find("x10 = 0x00000001") != std::string::npos,
            "symmetric-tree debe devolver true en x10/a0.");

    const auto quicksort = runExample("programas/quicksort.bin");
    require(quicksort.find("01 00 00 00 02 00 00 00 03 00 00 00 04 00 00 00") != std::string::npos,
            "quicksort debe ordenar los primeros cuatro valores.");
    require(quicksort.find("06 00 00 00 08 00 00 00 09 00 00 00") != std::string::npos,
            "quicksort debe ordenar los ultimos tres valores.");

    const auto fullSuite = runExample("pruebas/full-test.bin");
    require(fullSuite.find("x10 = 0x00000001") != std::string::npos,
            "full-test debe completar las comprobaciones RV32I y devolver a0=1.");
}

void zeroInstructionIsRejected() {
    TemporaryProgram program("riscv-simulator-invalid-instruction-test.bin");
    program.write({0u, 0u}, 1);
    Simulator simulator;
    require(simulator.loadProgram(program.path().string()), "No se pudo cargar la instruccion invalida.");
    bool rejected = false;
    try {
        simulator.step();
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    require(rejected, "La instruccion 0x00000000 debe rechazarse.");
    require(simulator.getPC() == 0, "El PC no debe avanzar tras una instruccion invalida.");
}

} // namespace

int main() {
    try {
        ebreakStopsOnItsAddress();
        ebreakStopsAfterEarlierInstructions();
        otherSystemInstructionsRemainUnsupported();
        zeroInstructionIsRejected();
        referenceExampleProgramsProduceExpectedResults();
        std::cout << "Todas las pruebas del simulador pasaron.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Fallo de prueba: " << error.what() << '\n';
        return 1;
    }
}
