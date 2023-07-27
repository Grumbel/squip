#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include <fmt/format.h>
#include <squip/unpack.hpp>
#include <squip/squirrel_util.hpp>
#include <squip/squirrel_vm.hpp>

TEST(SquipUnpack, unpack)
{
  squip::SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();

  sq_pushroottable(vm);
  sq_pushbool(vm, true);
  sq_pushinteger(vm, 45);
  sq_pushfloat(vm, 0.125);
  sq_pushstring(vm, "HelloWorld", -1);

  EXPECT_EQ(squip::unpack<SQBool>(vm, 2), true);
  EXPECT_EQ(squip::unpack<SQInteger>(vm, 3), 45);
  EXPECT_EQ(squip::unpack<SQFloat>(vm, 4), 0.125);
  EXPECT_STREQ(squip::unpack<char const*>(vm, 5), "HelloWorld");

  auto [a, b, c, d] = squip::unpack_args<SQBool, SQInteger, SQFloat, SQChar const*>(vm);
  EXPECT_EQ(a, true);
  EXPECT_EQ(b, 45);
  EXPECT_EQ(c, 0.125);
  EXPECT_STREQ(d, "HelloWorld");

  sq_pop(vm, 5);
}

/* EOF */


