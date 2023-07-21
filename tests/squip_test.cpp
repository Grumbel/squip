#include <gtest/gtest.h>

#include <squip/squirrel_util.hpp>
#include <squip/squirrel_vm.hpp>

TEST(SquipTest, to_string)
{
  squip::SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();

  sq_pushstring(vm, "helloworld", -1);
  sq_pushinteger(vm, 45);
  sq_pushfloat(vm, 0.125f);
  sq_pushbool(vm, true);
  sq_pushbool(vm, false);

  sq_newarray(vm, 0);
  sq_pushinteger(vm, 11); sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 22); sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 33); sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 44); sq_arrayappend(vm, -2);

  EXPECT_EQ(squip::to_string(vm, 1), "\"helloworld\"");
  EXPECT_EQ(squip::to_string(vm, 2), "45");
  EXPECT_EQ(squip::to_string(vm, 3), "0.125");
  EXPECT_EQ(squip::to_string(vm, 4), "true");
  EXPECT_EQ(squip::to_string(vm, 5), "false");
  EXPECT_EQ(squip::to_string(vm, 6), "[11, 22, 33, 44]");
}

TEST(SquipTest, print_stack)
{
  squip::SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();

  sq_pushstring(vm, "helloworld", -1);
  sq_pushinteger(vm, 45);
  sq_pushfloat(vm, 0.125f);
  sq_pushbool(vm, true);
  sq_pushbool(vm, false);

  sq_newarray(vm, 0);
  sq_pushinteger(vm, 11); sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 22); sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 33); sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 44); sq_arrayappend(vm, -2);

  char const* expected =
    "1: \"helloworld\"\n"
    "2: 45\n"
    "3: 0.125\n"
    "4: true\n"
    "5: false\n"
    "6: [11, 22, 33, 44]\n";

  std::ostringstream os;
  squip::print_stack(vm, os);
  EXPECT_EQ(os.str(), expected);
}

/* EOF */

