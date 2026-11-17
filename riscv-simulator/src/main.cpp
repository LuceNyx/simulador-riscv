#include "Simulator.hpp"
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static uint32_t parseAddress(const std::string& s) {
    std::size_t used = 0;
    const auto value = std::stoull(s, &used, 0);
    if (used != s.size() || value > 0xffffffffull)
        throw std::invalid_argument("Direccion invalida: " + s);
    return static_cast<uint32_t>(value);
}

static void help() {
    std::cout <<
        "Comandos disponibles:\n"
        "  step              Ejecuta una instruccion\n"
        "  run [limite]      Ejecuta varias instrucciones (por defecto 1000000)\n"
        "  pc                Muestra el PC\n"
        "  regs              Muestra x0-x31\n"
        "  reg xN            Muestra un registro (ej. reg x5)\n"
        "  mem INICIO FIN    Muestra bytes de memoria (hex o decimal)\n"
        "  reset             Reinicia PC y registros\n"
        "  help              Muestra esta ayuda\n"
        "  exit              Cierra el simulador\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: riscv-simulator <programa.bin>\n";
        return 1;
    }

    try {
        Simulator simulator;
        if (!simulator.loadProgram(argv[1])) {
            std::cerr << "No se pudo abrir el archivo: " << argv[1] << '\n';
            return 1;
        }

        std::cout << '"' << argv[1] << "\" cargado desde 0x00000000.\n";
        help();

        std::string line;
        while (std::cout << "> " && std::getline(std::cin, line)) {
            std::istringstream input(line);
            std::vector<std::string> args;
            std::string token;
            while (input >> token) args.push_back(token);
            if (args.empty()) continue;

            try {
                const auto& cmd = args[0];
                if (cmd == "exit" || cmd == "quit") break;
                else if (cmd == "help") help();
                else if (cmd == "pc") simulator.showPC();
                else if (cmd == "regs") simulator.showRegisters();
                else if (cmd == "reg") {
                    if (args.size() != 2 || args[1].size() < 2 || args[1][0] != 'x')
                        throw std::invalid_argument("Uso: reg xN");
                    simulator.showRegister(static_cast<unsigned>(std::stoul(args[1].substr(1))));
                }
                else if (cmd == "mem") {
                    if (args.size() != 3) throw std::invalid_argument("Uso: mem INICIO FIN");
                    simulator.showMemory(parseAddress(args[1]), parseAddress(args[2]));
                }
                else if (cmd == "step") simulator.step();
                else if (cmd == "run") {
                    uint64_t limit = args.size() >= 2 ? std::stoull(args[1]) : 1000000;
                    simulator.run(limit);
                }
                else if (cmd == "reset") {
                    simulator.reset();
                    std::cout << "PC y registros reiniciados; el programa sigue cargado.\n";
                }
                else std::cout << "Comando desconocido. Escribe 'help'.\n";
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error fatal: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
