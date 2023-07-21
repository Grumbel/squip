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

  EXPECT_EQ(squip::to_string(vm, 1), "\"helloworld\"");
  EXPECT_EQ(squip::to_string(vm, 2), "45");
  EXPECT_EQ(squip::to_string(vm, 3), "0.125");
  EXPECT_EQ(squip::to_string(vm, 4), "true");
  EXPECT_EQ(squip::to_string(vm, 5), "false");
}

/* EOF */

