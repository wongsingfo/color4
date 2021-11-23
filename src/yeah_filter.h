// Created by chengke on 2021/11/23

#ifndef COLOR4_YEAH_FILTER_H
#define COLOR4_YEAH_FILTER_H

#include <cstdint>
#include <iomanip>
#include <stdlib.h>
#include <stdint.h>
#include <type_traits>
#include <algorithm>

#include "utils.h"
#include "bit_table.h"
#include "city.h"

template<int bits_per_key, class Key, class Hash, int way = 4>
class YeahFilter 
{
public:
    static_assert(std::is_same<size_t, decltype(std::declval<Hash>()(std::declval<Key>()))>::value, 
            "The hash function needs to return size_t");

    YeahFilter(Hash hash, size_t max_num_keys)
        : hash_(hash)
    {
        num_buckets_ = upperpower2(std::max<size_t>(1, max_num_keys / way));
        table_ = new BitTable<bits_per_key, way>(num_buckets_);
    }

    ~YeahFilter() 
    {
        delete table_;
    }

    bool insert(const Key &v)
    {
        size_t h;
        size_t fp;
        generate_hash(v, &h, &fp);
        for (int i = 0; i < kMaxKick; i++) {
            bool kick = i > 3;
            uint32_t last;

            if (table_->insert_elem(h, fp, kick, &last)) {
                return true;
            }

            if (kick) {
                fp = last;
            }

            if (i & 1) {
                h = xor_hash_fp1(h, fp);
            } else {
                h = xor_hash_fp2(h, fp);
            }
        }
	return false;
    }

    bool query(const Key &v)
    {
        size_t h;
        size_t fp;
        generate_hash(v, &h, &fp);

        if (table_->query_elem(h, fp)) {
            return true;
        }
        size_t h2 = xor_hash_fp2(h, fp);
        if (table_->query_elem(h2, fp)) {
            return true;
        }
        size_t h3 = xor_hash_fp1(h2, fp);
        if (table_->query_elem(h3, fp)) {
            return true;
        }
        size_t h4 = xor_hash_fp1(h, fp);
        if (table_->query_elem(h4, fp)) {
            return true;
        }
        
        return false;
    }

    /*
    void remove(const Key &v) 
    {
        size_t h1;
        size_t fp;
        generate_hash(v, &h1, &fp);

        if (table_->delete_elem(h1, fp)) {
            return;
        }

        size_t h2 = xor_hash_fp2(h1, fp);
        if (table_->delete_elem(h2, fp)) {
            return;
        }
    }
    */

private:

    inline void generate_hash(const Key& key, size_t* h1, size_t* fp)
    {
        size_t h = hash_(key);

        size_t ha;
        if (sizeof(size_t) == 4) {
            // equivalent to (h % num_buckets_)
            ha = h & (num_buckets_ - 1);
        } else {
            ha = (h >> 32) & (num_buckets_ - 1);
        }
        *h1 = ha;

        size_t hb = h & ((1l << bits_per_key) - 1);
        if (hb == 0) {
            // 0 is reserved to mark the empty slot in the bucket.
            *fp = 1;
        } else {
            *fp = hb;
        }

        // log_debug("%s(%u) -> %zu %zu\n", __FUNCTION__, key, *h1, *fp);
    }

    inline size_t xor_hash_fp1(size_t h, size_t fp)
    {
        size_t h1 = (h ^ ((uint32_t) fp * 0x5bd1e995));
        return h1 & (num_buckets_ - 1);
    }

    inline size_t xor_hash_fp2(size_t h, size_t fp)
    {
        size_t hash_fp = CityHash64((const char*) &fp, sizeof(fp));
        size_t h1 = (h ^ ((uint32_t) hash_fp));
        return h1 & (num_buckets_ - 1);
    }

    Hash hash_;

    size_t num_buckets_;

    static constexpr int kMaxKick = 500;

    BitTable<bits_per_key, way> *table_;

};

#endif // COLOR4_YEAH_FILTER_H
