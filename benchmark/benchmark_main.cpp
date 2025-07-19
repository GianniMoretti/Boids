#include "benchmark_runner.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -o, --output FILE       Output CSV file (default: benchmark_results.csv)\n";
    std::cout << "  -c, --cycles N          Number of benchmark cycles (default: 20)\n";
    std::cout << "  -t, --timeout SEC       Timeout in seconds (default: 300)\n";
    std::cout << "  --quick                 Quick test (fewer boids and threads)\n";
    std::cout << "  --medium                Medium test (more threads)\n";
    std::cout << "  --full                  Full test (all configurations)\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << program_name << " --quick -o quick_results.csv\n";
    std::cout << "  " << program_name << " --full -c 30 -t 600\n";
}

BenchmarkConfig parseArguments(int argc, char* argv[]) {
    BenchmarkConfig config;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                config.output_file = argv[++i];
            } else {
                std::cerr << "Error: " << arg << " requires a filename\n";
                exit(1);
            }
        }
        else if (arg == "-c" || arg == "--cycles") {
            if (i + 1 < argc) {
                config.profile_cycles = std::atoi(argv[++i]);
                if (config.profile_cycles <= 0) {
                    std::cerr << "Error: cycles must be positive\n";
                    exit(1);
                }
            } else {
                std::cerr << "Error: " << arg << " requires a number\n";
                exit(1);
            }
        }
        else if (arg == "-t" || arg == "--timeout") {
            if (i + 1 < argc) {
                config.timeout_seconds = std::atoi(argv[++i]);
                if (config.timeout_seconds <= 0) {
                    std::cerr << "Error: timeout must be positive\n";
                    exit(1);
                }
            } else {
                std::cerr << "Error: " << arg << " requires a number\n";
                exit(1);
            }
        }
        else if (arg == "--quick") {
            config.boids_counts = {1000, 2000, 4000, 8000};
            config.thread_counts = {1, 2, 4, 6};
            config.profile_cycles = 10;
        }
        else if (arg == "--medium") {
            config.boids_counts = {2000, 4000, 8000, 16000};
            config.thread_counts = {1, 2, 4, 6, 8, 12};
            config.profile_cycles = 15;
        }
        else if (arg == "--full") {
            config.boids_counts = {1000, 2000, 4000, 8000, 16000, 32000};
            config.thread_counts = {1, 2, 4, 6, 8, 12, 16};
            config.profile_cycles = 30;
        }
        else {
            std::cerr << "Error: unknown option " << arg << "\n";
            printUsage(argv[0]);
            exit(1);
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    // Inizializza random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Parse arguments
    BenchmarkConfig config = parseArguments(argc, argv);
    
    std::cout << "===========================================\n";
    std::cout << "BOIDS PERFORMANCE BENCHMARK SUITE (C++)\n";
    std::cout << "===========================================\n";
    std::cout << "Configuration:\n";
    std::cout << "  Output file: " << config.output_file << "\n";
    std::cout << "  Benchmark cycles: " << config.profile_cycles << "\n";
    std::cout << "  Timeout: " << config.timeout_seconds << "s\n";
    std::cout << "  Boids counts: ";
    for (size_t i = 0; i < config.boids_counts.size(); i++) {
        std::cout << config.boids_counts[i];
        if (i < config.boids_counts.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";
    std::cout << "  Thread counts: ";
    for (size_t i = 0; i < config.thread_counts.size(); i++) {
        std::cout << config.thread_counts[i];
        if (i < config.thread_counts.size() - 1) std::cout << ", ";
    }
    std::cout << "\n===========================================\n\n";
    
    try {
        // Create and run benchmark
        BenchmarkRunner runner(config);
        runner.runAllBenchmarks();
        
        // Save results and print summary
        runner.saveResults();
        runner.printSummary();
        
        std::cout << "\n===========================================\n";
        std::cout << "BENCHMARK COMPLETATO!\n";
        std::cout << "===========================================\n";
        std::cout << "Per analizzare i risultati, esegui:\n";
        std::cout << "  python3 analyze_results.py " << config.output_file << "\n";
        std::cout << "===========================================\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\nERRORE FATALE: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\nERRORE FATALE: Eccezione sconosciuta" << std::endl;
        return 1;
    }
}
