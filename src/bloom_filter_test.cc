// Created by chengke on 2021/11/06
#include "bloom_filter.h"
#include "gtest/gtest.h"

class BloomFilterTest : public ::testing::Test
{

public:
    static size_t hash_id(int x) {
        return x;
    }

    static size_t hash_c23(int x) {
        return x * 23 * x;
    }
};

TEST_F(BloomFilterTest, OneElement)
{
    BloomFilter<17, 1, int, size_t (*)(int)> filter{hash_id};
    filter.insert(1000);
    EXPECT_TRUE(filter.query(1000));
    EXPECT_FALSE(filter.query(2048));
    EXPECT_FALSE(filter.query(100));
}

TEST_F(BloomFilterTest, TwoHash)
{
    BloomFilter<17, 2, int, size_t (*)(int)> filter{hash_id, hash_c23};
    filter.insert(1000);
    EXPECT_TRUE(filter.query(1000));
    EXPECT_FALSE(filter.query(2048));
    EXPECT_FALSE(filter.query(100));
}

TEST_F(BloomFilterTest, TwoHashFiveElements)
{
    BloomFilter<31, 2, int, size_t (*)(int)> filter{hash_id, hash_c23};
    filter.insert(1000);
    filter.insert(1001);
    filter.insert(1002);
    filter.insert(1003);
    filter.insert(1004);
    EXPECT_TRUE(filter.query(1000));
    EXPECT_TRUE(filter.query(1002));
    EXPECT_FALSE(filter.query(2048));
    EXPECT_FALSE(filter.query(100));
}

