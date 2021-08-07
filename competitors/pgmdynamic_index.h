#ifndef SOSD_PGMDYNAMIC_INDEX_H
#define SOSD_PGMDYNAMIC_INDEX_H

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "../util.h"
#include "base.h"
//#include "pgm_index.hpp"
//#include "pgm_index_dynamic.hpp"
#include "./PGM-index/include/pgm/pgm_index.hpp"
#include "./PGM-index/include/pgm/pgm_index_dynamic.hpp"
//#include <functional>
//#include <boost/iterator/transform_iterator.hpp>
//#include <boost/range/adaptors.hpp>

#define ValueType uint64_t

template<class KeyType, int pgm_error>
class DynamicPGM : public Competitor {
public:
    uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
        const auto extract_key = [](KeyValue<KeyType> kv) {
            auto key = kv.key, value = kv.value;
            return std::make_pair(key, value);
        };

        // This code uses a boost transform iterator to avoid a copy. It
        // seems to be much slower, however.
        /*
        auto it_begin = boost::make_transform_iterator(data.begin(), extract_key);
        auto it_end = boost::make_transform_iterator(data.end(), extract_key);
        pgm_ = PGMIndex<KeyType, pgm_error, 4>(it_begin, it_end);
        */

        // don't count the data copy time against the PGM build time
        std::vector<std::pair<KeyType, ValueType>> keys;
        keys.reserve(data.size());
        std::transform(data.begin(), data.end(), std::back_inserter(keys),
                       extract_key);

        uint64_t build_time =
                util::timing([&] { dpgm_ = decltype(dpgm_)(keys.begin(), keys.end()); });

        return build_time;
    }

    SearchBound EqualityLookup(const KeyType lookup_key) const {
        auto pos = dpgm_.find(lookup_key);
        auto lo = pos->value(), hi = pos->value();
        return (SearchBound){ lo, hi };
    }

    template <typename KT>
    uint64_t Search(const std::vector<EqualityLookupStructure<KeyType>>& data) {

        return util::timing(
                [&] {
                    for (unsigned int idx = 0; idx <  data.size(); ++idx) {
                        // Compute the actual index for debugging.
                        const uint64_t lookup_key = data[idx].key;
                        dpgm_.find(lookup_key);
                    }
                });
    }

    template<typename KT>
    uint64_t Insert(const std::vector<KeyValue<KT>> &data) {
        return util::timing(
                [&] {
                    for (auto kv : data) {
                        dpgm_.insert_or_assign(kv.key, kv.value);
                    }
                });
    }

    std::string name() const { return "DynamicPGM"; }

    std::size_t size() const { return dpgm_.size_in_bytes(); }

    bool applicable(bool unique, const std::string& data_filename) const {
        return true;
    }

    int variant() const { return pgm_error; }

    bool insertion_possible() const {
        return true;
    }

private:
    pgm::DynamicPGMIndex<KeyType, ValueType, pgm::PGMIndex<KeyType, pgm_error, 4>> dpgm_
};

#endif //SOSD_PGMDYNAMIC_INDEX_H
