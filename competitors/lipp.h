#pragma once

#include <utility>

#include "./LIPP/src/core/lipp.h"
#include "./LIPP/src/core/lipp_base.h"
#include "base.h"
#include "../util.h"

template <class KeyType, int size_scale>
class LIPP : public Competitor {
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    std::vector<std::pair<KeyType, uint64_t>> loading_data;
    loading_data.reserve(data.size());
    // We use ALEX as a non-clustered index by only inserting every n-th entry.
    // n is defined by size_scale.
    for (auto& itm : data) {
      uint64_t idx = itm.value;
      if (size_scale > 1 && idx % size_scale != 0) continue;
      loading_data.push_back(std::make_pair(itm.key, itm.value));
    }

    data_size_ = data.size();

    return util::timing(
        [&] { lipp_.bulk_load(loading_data.data(), loading_data.size()); });
  }

  SearchBound EqualityLookup(const KeyType lookup_key) const {
        //std::cout << "Looking up key: " << lookup_key << std::endl;

        auto payload = lipp_.at(lookup_key);

        return (SearchBound){payload, payload};
  }

    template <typename KT>
    uint64_t Search(const std::vector<EqualityLookupStructure<KeyType>>& data) {

        return util::timing(
                [&] {
                    for (unsigned int idx = 0; idx <  data.size(); ++idx) {
                        // Compute the actual index for debugging.
                        const uint64_t lookup_key = data[idx].key;
                        lipp_.at(lookup_key);
                    }
                });
    }

  template <typename KT>
  uint64_t Insert(const std::vector<EqualityLookupStructure<KeyType>>& data) {

      return util::timing(
              [&] {
                  for (unsigned int idx = 0; idx <  data.size(); ++idx) {
                      //std::cout << "Inserting key: " << data[idx].key << " with payload: " << data[idx].result << std::endl;
                      lipp_.insert(data[idx].key, data[idx].result);
                  }
                  //std::cout << "Finished inserting keys" << std::endl;
              });
  }

  std::string name() const { return "LIPP"; }

  std::size_t size() const {
      return lipp_.index_size();
  }

  int variant() const { return size_scale; }

  bool insertion_possible() const { return true; }

 private:
  uint64_t data_size_ = 0;
  LIPP<KeyType, uint64> lipp_;
};
