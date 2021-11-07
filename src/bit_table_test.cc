// Created by chengke on 2021/11/06
#include "bit_table.h"
#include "gtest/gtest.h"
#include <cstdint>

class BitTableTest : public ::testing::Test
{

public:
    static size_t hash_id(int x) {
        return x;
    }

    static size_t hash_c23(int x) {
        return x * 23 * x;
    }
};

TEST_F(BitTableTest, Bit)
{
    BitTable<2> table(5);
    table.set_elem(4, 0, 1);
    EXPECT_EQ(table.get_elem(4, 0), 1);
    EXPECT_TRUE(table.query_elem(4, 1));
    table.set_elem(4, 0, 0);
    EXPECT_EQ(table.get_elem(4, 0), 0);
    EXPECT_FALSE(table.query_elem(4, 1));
    EXPECT_FALSE(table.delete_elem(4, 1));
}

TEST_F(BitTableTest, OneElement)
{
    BitTable<4> table(5);
    uint32_t last;
    EXPECT_TRUE(table.insert_elem(7, 1, false, &last));
    EXPECT_TRUE(table.query_elem(7, 1));
    EXPECT_FALSE(table.query_elem(7, 2));
    EXPECT_FALSE(table.query_elem(7, 3));
    EXPECT_FALSE(table.query_elem(6, 1));
    EXPECT_FALSE(table.query_elem(6, 3));
    EXPECT_TRUE(table.delete_elem(7, 1));
    EXPECT_FALSE(table.query_elem(7, 1));
}

TEST_F(BitTableTest, Kick)
{
    BitTable<4> table(5);
    uint32_t last = 0;
    EXPECT_TRUE(table.insert_elem(7, 1, true, &last));
    EXPECT_TRUE(table.insert_elem(7, 2, true, &last));
    EXPECT_TRUE(table.insert_elem(7, 3, true, &last));
    EXPECT_TRUE(table.insert_elem(7, 4, true, &last));
    EXPECT_FALSE(table.insert_elem(7, 5, true, &last));
    EXPECT_NE(0, last);
    EXPECT_FALSE(table.insert_elem(7, 2, false, &last));
}
