#ifndef BENCHMARK_RUNNER_H
#define BENCHMARK_RUNNER_H

#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <functional>

struct BenchmarkResult {
    std::string implementation;
    int boids;
    int threads;
    double meanTime;
    bool success;
    
    BenchmarkResult(const std::string& impl, int b, int t, double time, bool succ = true)
        : implementation(impl), boids(b), threads(t), meanTime(time), success(succ) {}
};

struct BenchmarkConfig {
    std::vector<int> boids_counts = {1000, 2000, 4000, 8000, 16000, 32000};
    std::vector<int> thread_counts = {1, 2, 4, 6, 8, 12};
    int profile_cycles = 20;
    int timeout_seconds = 300;
    std::string output_file = "benchmark_results.csv";
};

class BenchmarkRunner {
private:
    BenchmarkConfig config;
    std::vector<BenchmarkResult> results;
    
    // Funzione per eseguire il benchmark di una singola implementazione
    double runSingleBenchmark(std::function<void()> setup_func, 
                             std::function<void()> bench_func,
                             int cycles);
    
    // Timeout wrapper
    template<typename Func>
    bool runWithTimeout(Func func, int timeout_sec, double& result);

public:
    BenchmarkRunner(const BenchmarkConfig& cfg = BenchmarkConfig()) : config(cfg) {}
    
    // Metodi per testare le diverse implementazioni
    void benchmarkAoS();
    void benchmarkParallel();
    
    // Esegue tutti i benchmark
    void runAllBenchmarks();
    
    // Salva i risultati
    void saveResults() const;
    void printSummary() const;
    
    // Getters
    const std::vector<BenchmarkResult>& getResults() const { return results; }
};

#endif // BENCHMARK_RUNNER_H
