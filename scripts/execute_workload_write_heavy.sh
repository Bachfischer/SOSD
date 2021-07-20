#! /usr/bin/env bash

echo "Executing benchmark and saving results..."

BENCHMARK=build/benchmark
if [ ! -f $BENCHMARK ]; then
    echo "benchmark binary does not exist"
    exit
fi

function do_benchmark() {

    RESULTS=./results/results_write_heavy_$1.txt
    if [ -f $RESULTS ]; then
        echo "Already have results for $1"
    else
        echo "Executing workload $1"
        for index in ALEX BTree DPGM;
        do
          $BENCHMARK -r 1 ./data/$1 ./data/$1_equality_lookups_2M --inserts ./data/$1_inserts_18M --pareto --only $index | tee -a ./results/results_write_heavy_$1.txt
        done
    fi
}

mkdir -p ./results

for dataset in $(cat scripts/datasets_under_test.txt); do
    do_benchmark "$dataset"
done
