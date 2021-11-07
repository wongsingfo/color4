// Created by chengke on 2021/11/07
#include "cuckoo_filter.h"
#include "gtest/gtest.h"
#include <cstdint>

class CuckooFilterTest : public ::testing::Test
{

public:
    static size_t hash_id(int x) {
        return x;
    }

    static size_t hash_c23(int x) {
        return x * 23 * x * 123498711111;
    }
};

TEST_F(CuckooFilterTest, OneElement)
{
    CuckooFilter<2, int, size_t (*)(int)> filter(hash_c23, 100);
    const int e1 = 9823147;
    const int e2 = 4231678;
    filter.insert(e1);
    EXPECT_TRUE(filter.query(e1));
    EXPECT_FALSE(filter.query(e2));
    filter.remove(e1);
    EXPECT_FALSE(filter.query(e1));
    EXPECT_FALSE(filter.query(e2));
}
