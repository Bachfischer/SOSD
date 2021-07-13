#! /usr/bin/env bash

echo "Executing benchmark and saving results..."

BENCHMARK=build/benchmark
if [ ! -f $BENCHMARK ]; then
    echo "benchmark binary does not exist"
    exit
fi

function do_benchmark() {

    RESULTS=./results/$1_results_$2_inserts.txt
    if [ -f $RESULTS ]; then
        echo "Already have results for $1"
    else
        echo "Executing workload $1"
        for index in BTree ALEX;
        do
          $BENCHMARK -r 1 ./data/$1 ./data/$1_equality_lookups_10M --inserts ./data/$1_inserts_$2 --pareto --only $index | tee -a ./results/$1_results_$2_inserts.txt
        done
    fi
}

mkdir -p ./results

for dataset in $(cat scripts/datasets_under_test.txt); do
    do_benchmark "$dataset" "1M"
    # do_benchmark "$dataset" "10M"
done
