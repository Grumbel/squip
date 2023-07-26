#include <gtest/gtest.h>

#include <iostream>
#include <squip/squirrel_util.hpp>
#include <squip/squirrel_vm.hpp>

TEST(SquipTableContext, store)
{
  squip::SquirrelVM sqvm;

  squip::TableContext root = sqvm.get_roottable();

  root.store_bool("boolvalue", true);
  root.store_int("intvalue", 45);
  root.store_float("floatvalue", 0.125);
  root.store_string("stringvalue", "StringValue");

  EXPECT_EQ(root.read_bool("boolvalue"), true);
  EXPECT_EQ(root.read_int("intvalue"), 45);
  EXPECT_EQ(root.read_float("floatvalue"), 0.125);
  EXPECT_EQ(root.read_string("stringvalue"), "StringValue");

  root.end();
}

/* EOF */
