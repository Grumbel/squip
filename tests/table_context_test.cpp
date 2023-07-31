#include <gtest/gtest.h>

#include <iostream>
#include <squip/util.hpp>
#include <squip/squirrel_vm.hpp>

TEST(SquipTableContext, store)
{
  squip::SquirrelVM sqvm;

  squip::TableContext root = sqvm.push_roottable();

  root.store_bool("boolvalue", true);
  root.store_int("intvalue", 45);
  root.store_float("floatvalue", 0.125);
  root.store_string("stringvalue", "StringValue");

  ASSERT_EQ(root.get_bool("boolvalue"), true);
  ASSERT_EQ(root.get_int("intvalue"), 45);
  ASSERT_EQ(root.get_float("floatvalue"), 0.125);
  ASSERT_EQ(root.get_string("stringvalue"), "StringValue");

  bool boolvalue = {};
  int intvalue = {};
  float floatvalue = {};
  std::string stringvalue = {};

  ASSERT_TRUE(root.read_bool("boolvalue", boolvalue));
  ASSERT_TRUE(root.read_int("intvalue", intvalue));
  ASSERT_TRUE(root.read_float("floatvalue", floatvalue));
  ASSERT_TRUE(root.read_string("stringvalue", stringvalue));

  ASSERT_EQ(boolvalue, true);
  ASSERT_EQ(intvalue, 45);
  ASSERT_EQ(floatvalue, 0.125);
  ASSERT_EQ(stringvalue, "StringValue");

  root.create_table("mytable");
  ASSERT_TRUE(root.has_key("mytable"));
  sq_poptop(sqvm.get_vm());

  root.create_or_get_table("mytable1");
  ASSERT_TRUE(root.has_key("mytable1"));
  sq_poptop(sqvm.get_vm());

  root.create_or_get_table("mytable");
  ASSERT_TRUE(root.has_key("mytable"));
  sq_poptop(sqvm.get_vm());

  root.rename_entry("mytable", "mytablerenamed");
  ASSERT_TRUE(root.has_key("mytablerenamed"));
  ASSERT_FALSE(root.has_key("mytable"));

  root.delete_entry("mytablerenamed");
  ASSERT_FALSE(root.has_key("mytablerenamed"));

  {
    squip::TableContext tbl = root.create_table("newtable");

    tbl.store_bool("tbl_boolvalue", true);
    tbl.store_int("tbl_intvalue", 45);
    tbl.store_float("tbl_floatvalue", 0.125);
    tbl.store_string("tbl_stringvalue", "StringValue");

    ASSERT_EQ(tbl.get_bool("tbl_boolvalue"), true);
    ASSERT_EQ(tbl.get_int("tbl_intvalue"), 45);
    ASSERT_EQ(tbl.get_float("tbl_floatvalue"), 0.125);
    ASSERT_EQ(tbl.get_string("tbl_stringvalue"), "StringValue");

    sq_poptop(sqvm.get_vm());
  }

  sq_poptop(sqvm.get_vm());
}

/* EOF */
