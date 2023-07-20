#include <gtest/gtest.h>

#include <squip/squirrel_util.hpp>
#include <squip/squirrel_vm.hpp>

using namespace squip;

TEST(SquipTest, squirrel2string)
{
  SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();

  sq_pushstring(vm, "helloworld", -1);
  sq_pushinteger(vm, 45);
  sq_pushfloat(vm, 0.125f);
  sq_pushbool(vm, true);
  sq_pushbool(vm, false);

  EXPECT_EQ(squirrel2string(vm, 1), "\"helloworld\"");
  EXPECT_EQ(squirrel2string(vm, 2), "45");
  EXPECT_EQ(squirrel2string(vm, 3), "0.125");
  EXPECT_EQ(squirrel2string(vm, 4), "true");
  EXPECT_EQ(squirrel2string(vm, 5), "false");
}

/* EOF */

