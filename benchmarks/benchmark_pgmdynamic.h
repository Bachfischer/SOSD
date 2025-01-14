#pragma once
#include "benchmark.h"

template<template<typename> typename Searcher>
void benchmark_32_dpgm(sosd::Benchmark<uint32_t, Searcher>& benchmark, bool pareto);

template<template<typename> typename Searcher>
void benchmark_64_dpgm(sosd::Benchmark<uint64_t, Searcher>& benchmark, bool pareto);
