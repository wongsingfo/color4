// Created by chengke on 2021/11/06

#ifndef COLOR4_BLOOM_FILTER_H
#define COLOR4_BLOOM_FILTER_H

#include <cstdint>
#include <initializer_list>
#include <tuple>
#include <string.h>
#include <cassert>

/// Get the type of the N-th element.
template<int N, class... Args>
using get_type = typename std::tuple_element<N, std::tuple<Args...>>::type;

/// Get the value of the N-th element.
template<int N, class... Args>
constexpr decltype(auto) get(Args&&... args)
{
    return std::get<N>(std::forward_as_tuple(args...));
}

/// Get the length of Args.
template<class... Args>
constexpr auto length_of = sizeof...(Args);

constexpr auto round_up(int x, int align)
{
    return (x + align - 1) / align;
}

/**
 * @param n    Number of bits in one bloom filter.
 * @param k    Number of hash functions.
 * @param Key  The type of elements. It can be integer, string, etc..
 * @param Hash The hash function that takes Key as input and outputs a size_t.
 */
template<size_t n, int k, class Key, class Hash>
class BloomFilter
{
public:
    static_assert(std::is_same<size_t, decltype(std::declval<Hash>()(std::declval<Key>()))>::value, 
            "The hash function needs to return size_t");

    BloomFilter(std::initializer_list<Hash> hash)
    {
        memset(bf_, 0, sizeof(bf_));
        assert(hash.size() == k && "Number of hash function unmatched");

        int i = 0;
        for (auto &h : hash) {
            hash_[i] = h;
            i += 1;
        }
    }

    void insert(const Key &key)
    {
        for (int i = 0; i < k; i++) {
            size_t p = hash_[i](key) % n;
            bf_[p / kBitsPerByte] |= 1u << (p % kBitsPerByte);
        }
    }

    /**
     * Query whether key is in the bloom filter. It may produce false positive.
     * @returns True if key is in the bloom filter.
     */
    bool query(const Key &key)
    {
        for (int i = 0; i < k; i++) {
            size_t p = hash_[i](key) % n;
            if (not (bf_[p / kBitsPerByte] & (1u << (p % kBitsPerByte)))) {
                return false;
            }
        }
        return true;
    }
    
private:

    static constexpr int kBitsPerByte = 8;

    Hash hash_[k];

    uint8_t bf_[round_up(n, kBitsPerByte)];

};

#endif // COLOR4_BLOOM_FILTER_H
