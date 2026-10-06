#include "pch.h"

#include "Collections/collection.h"

#include <gtest/gtest.h>

namespace xsim::aircrafts::tests
{
    TEST(CollectionTests, CollectionStartsEmpty)
    {
        Collection<int> values;

        EXPECT_TRUE(values.empty());
        EXPECT_EQ(values.size(), 0u);
    }

    TEST(CollectionTests, CollectionCanAddItem)
    {
        Collection<int> values;

        values.add(42);

        EXPECT_FALSE(values.empty());
        EXPECT_EQ(values.size(), 1u);
    }

    TEST(CollectionTests, CollectionCanBeEnumerated)
    {
        Collection<int> values;
        values.add(10);
        values.add(20);

        int total = 0;

        for (const auto value : values)
        {
            total += value;
        }

        EXPECT_EQ(total, 30);
    }
}
