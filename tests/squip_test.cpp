#include <squip/squip.hpp>

#include <gtest/gtest.h>

TEST(SquipTest, hello_world)
{
  ASSERT_EQ(squip::hello_world(), "hello world");
}

/* EOF */

