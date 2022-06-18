#include <benchmark/benchmark.h>

namespace autocrypt
{
static void Sample(benchmark::State &state)
{
    try
    {
        for (auto _ : state)
        {
			std::string a = "A";
        }

        state.counters["OPS"] = benchmark::Counter(state.iterations(), benchmark::Counter::kIsRate);
        state.counters["Latency"] = benchmark::Counter(state.iterations(), benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
    }
    catch (const std::exception &e)
    {
        state.SkipWithError(e.what());
    }
}
BENCHMARK(Sample)->UseRealTime();
}