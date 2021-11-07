// Created by chengke on 2021/11/07

#ifndef COLOR4_BIT_TABLE_H
#define COLOR4_BIT_TABLE_H

#include <cstdint>
#include <math.h>
#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>
#include <random>

#include "utils.h"

/**
 * @param bits_per_elem Number of bits of an element.
 */
template<int bits_per_elem, int elems_per_bucket = 4>
class BitTable
{

public: 
    using elem_type = uint32_t;

    static_assert(bits_per_elem <= sizeof(elem_type),
            "bits_per_elem is too large");
    static_assert(bits_per_elem > 0,
            "bits_per_elem must be greater than zero");

    class Bucket
    {
    public:
        uint8_t bits_[BitTable::kBytesPerBucket];
    } __attribute__((__packed__));

    BitTable(size_t num_buckets)
        : num_buckets_(num_buckets),
          rng_(kRngSeed)
    {
        size_t size = num_buckets_ + kBucketsPadding;
        log_debug("table size: %zu, bucket size: %zu\n", size, kBytesPerBucket);
        bucket_ = new Bucket[size];
        static_assert(sizeof(Bucket) == kBytesPerBucket,
                "Bucket is not in packed form");
        memset(bucket_, 0, size * sizeof(Bucket)); 
    }

    ~BitTable() 
    {
        delete[] bucket_;
    }

    /**
     * Get the j-th element in the i-th bucket.
     * Adapted from: https://github.com/efficient/cuckoofilter/blob/aac6569cf30f0dfcf39edec1799fc3f8d6f594da/src/singletable.h
     */
    inline uint32_t get_elem(size_t i, int j) const
    {
        const uint8_t *p = bucket_[i].bits_;
        uint32_t tag;
        /* following code only works for little-endian */
        if (bits_per_elem == 2) {
            p += (j >> 2);
            tag = *((uint8_t *)p) >> (j * 2);
        } else if (bits_per_elem == 4) {
            p += (j >> 1);
            tag = *((uint8_t *)p) >> ((j & 1) << 2);
        } else if (bits_per_elem == 8) {
            p += j;
            tag = *((uint8_t *)p);
        } else if (bits_per_elem == 12) {
            p += j + (j >> 1);
            tag = *((uint16_t *)p) >> ((j & 1) << 2);
        } else if (bits_per_elem == 16) {
            p += (j << 1);
            tag = *((uint16_t *)p);
        } else if (bits_per_elem == 32) {
            tag = ((uint32_t *)p)[j];
        } else {
            throw std::runtime_error("unsupported bits_per_elem");
	}
        uint32_t result = tag & kElemMask;
        log_debug("get(%zd, %d)=%d\n", i, j, result);
        return result;
    }

    /**
     * Insert element t into the j-th slot in the i-th bucket.
     * @param t    The element to insert. It will be truncated to bits_per_elem bits.
     */
    inline void set_elem(size_t i, int j, elem_type t)
    {
        log_debug("set(%zd, %d)=%d\n", i, j, t);
        uint8_t *p = bucket_[i].bits_;
        uint32_t elem = t & kElemMask;
        /* following code only works for little-endian */
        if (bits_per_elem == 2) {
            p += (j >> 2);
            *((uint8_t *)p) &= ~(kElemMask << (2 * j));
            *((uint8_t *)p) |= elem << (2 * j);
        } else if (bits_per_elem == 4) {
            p += (j >> 1);
            if ((j & 1) == 0) {
                *((uint8_t *)p) &= 0xf0;
                *((uint8_t *)p) |= elem;
            } else {
                *((uint8_t *)p) &= 0x0f;
                *((uint8_t *)p) |= (elem << 4);
            }
        } else if (bits_per_elem == 8) {
            ((uint8_t *)p)[j] = elem;
        } else if (bits_per_elem == 12) {
            p += (j + (j >> 1));
            if ((j & 1) == 0) {
                ((uint16_t *)p)[0] &= 0xf000;
                ((uint16_t *)p)[0] |= elem;
            } else {
                ((uint16_t *)p)[0] &= 0x000f;
                ((uint16_t *)p)[0] |= (elem << 4);
            }
        } else if (bits_per_elem == 16) {
            ((uint16_t *)p)[j] = elem;
        } else if (bits_per_elem == 32) {
            ((uint32_t *)p)[j] = elem;
        }
    }

    /**
     * Insert an element into the table at the i-th bucket.
     * @param last   The element that is kickout out by the newly inserted element.
     * @return True if there are availble space. False otherwise.
     */
    inline bool insert_elem(size_t i, elem_type elem, bool kick, elem_type *last) 
    {
        for (int j = 0; j < elems_per_bucket; j++) {
            if (get_elem(i, j) == 0) {
                set_elem(i, j, elem);
                return true;
            }
        }
        if (kick) {
            int j = this->rand() % elems_per_bucket;
            *last = get_elem(i, j);
            set_elem(i, j, elem);
        }
        return false;
    }

    /**
     * @param elem  The element to delete. It will be truncated to bits_per_elem bits.
     * @return True if elem is found and deleted.
     */
    inline bool delete_elem(size_t i, elem_type elem) 
    {
        elem &= kElemMask;
        for (int j = 0; j < elems_per_bucket; j++) {
            if (get_elem(i, j) == elem) {
                set_elem(i, j, 0);
                return true;
            }
        }
        return false;
    }

    inline bool query_elem(size_t i, elem_type elem)
    {
        elem &= kElemMask;
        for (int j = 0; j < elems_per_bucket; j++) {
            if (get_elem(i, j) == elem) {
                return true;
            }
        }
        return false;
    }

private:
    static constexpr size_t kBytesPerBucket =
        round_up(bits_per_elem * elems_per_bucket, 8) / 8;

    static constexpr size_t kBucketsPadding =
	((((kBytesPerBucket + 7) / 8) * 8) - 1) / kBytesPerBucket;

    static constexpr elem_type kElemMask = (1 << bits_per_elem) - 1;

    static constexpr unsigned kRngSeed = 20211107;

    size_t num_buckets_;
    
    Bucket *bucket_;

    // Generator of 32-bit numbers.
    std::mt19937 rng_;

    inline uint32_t rand()
    {
        return rng_();
    }
};

#endif // COLOR4_BIT_TABLE_H
