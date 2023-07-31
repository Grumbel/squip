#include <gtest/gtest.h>

#include <iostream>
#include <squip/util.hpp>
#include <squip/squirrel_vm.hpp>

TEST(SquipTableContext, store)
{
  squip::SquirrelVM sqvm;

  squip::TableContext root = sqvm.stack().push_roottable();

  root.store("boolvalue", true);
  root.store("intvalue", 45);
  root.store("floatvalue", 0.125f);
  root.store("stringvalue", "StringValue");

  ASSERT_EQ(root.get<bool>("boolvalue"), true);
  ASSERT_EQ(root.get<int>("intvalue"), 45);
  ASSERT_EQ(root.get<float>("floatvalue"), 0.125);
  ASSERT_EQ(root.get<std::string>("stringvalue"), "StringValue");
  ASSERT_STREQ(root.get<SQChar const*>("stringvalue"), "StringValue");

  bool boolvalue = {};
  int intvalue = {};
  float floatvalue = {};
  std::string stringvalue = {};

  ASSERT_TRUE(root.read<bool>("boolvalue", boolvalue));
  ASSERT_TRUE(root.read<int>("intvalue", intvalue));
  ASSERT_TRUE(root.read<float>("floatvalue", floatvalue));
  ASSERT_TRUE(root.read<std::string>("stringvalue", stringvalue));

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

    tbl.store("tbl_boolvalue", true);
    tbl.store("tbl_intvalue", 45);
    tbl.store("tbl_floatvalue", 0.125f);
    tbl.store("tbl_stringvalue", "StringValue");

    ASSERT_EQ(tbl.get<bool>("tbl_boolvalue"), true);
    ASSERT_EQ(tbl.get<int>("tbl_intvalue"), 45);
    ASSERT_EQ(tbl.get<float>("tbl_floatvalue"), 0.125);
    ASSERT_EQ(tbl.get<std::string>("tbl_stringvalue"), "StringValue");
    ASSERT_STREQ(tbl.get<SQChar const*>("tbl_stringvalue"), "StringValue");

    sq_poptop(sqvm.get_vm());
  }

  sq_poptop(sqvm.get_vm());
}

/* EOF */
