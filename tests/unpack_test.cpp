#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include <fmt/format.h>

#include <squip/squirrel_vm.hpp>
#include <squip/unpack.hpp>
#include <squip/util.hpp>

TEST(SquipUnpack, unpack)
{
  squip::SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();

  sq_pushroottable(vm);
  sq_pushbool(vm, true);
  sq_pushinteger(vm, 45);
  sq_pushfloat(vm, 0.125);
  sq_pushstring(vm, "HelloWorld", -1);

  sq_newarray(vm, 0);
  sq_pushinteger(vm, 11);
  sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 22);
  sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 33);
  sq_arrayappend(vm, -2);

  EXPECT_EQ(squip::unpack<SQBool>(vm, 2), true);
  EXPECT_EQ(squip::unpack<SQInteger>(vm, 3), 45);
  EXPECT_EQ(squip::unpack<SQFloat>(vm, 4), 0.125);
  EXPECT_STREQ(squip::unpack<char const*>(vm, 5), "HelloWorld");
  EXPECT_EQ(squip::unpack<std::vector<SQInteger> >(vm, 6),
            std::vector<SQInteger>({11, 22, 33}));

  auto [a, b, c, d, e] = squip::unpack_args<SQBool,
                                            SQInteger,
                                            SQFloat,
                                            SQChar const*,
                                            std::vector<SQInteger>>(vm);
  EXPECT_EQ(a, true);
  EXPECT_EQ(b, 45);
  EXPECT_EQ(c, 0.125);
  EXPECT_STREQ(d, "HelloWorld");
  EXPECT_EQ(e, std::vector<SQInteger>({11, 22, 33}));

  sq_pop(vm, 6);
}

/* EOF */


