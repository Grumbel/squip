#include <gtest/gtest.h>

#include <iostream>

#include <squip/array_context.hpp>
#include <squip/util.hpp>
#include <squip/unpack.hpp>
#include <squip/squirrel_vm.hpp>

TEST(SquipArrayContext, store)
{
  squip::SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();

  sq_newarray(vm, 0);
  squip::ArrayContext array_ctx(vm, -1);

  ASSERT_EQ(array_ctx.size(), 0);
  array_ctx.append(11);
  array_ctx.append(22);
  array_ctx.append(33);
  ASSERT_EQ(array_ctx.size(), 3);
  ASSERT_EQ(sq_gettop(vm), 1);
  ASSERT_EQ(squip::unpack<std::vector<SQInteger>>(vm, -1),
            (std::vector<SQInteger>{11, 22, 33}));

  array_ctx.insert(2, 27);
  ASSERT_EQ(squip::unpack<std::vector<SQInteger>>(vm, -1),
            (std::vector<SQInteger>{11, 22, 27, 33}));

  array_ctx.reverse();
  ASSERT_EQ(squip::unpack<std::vector<SQInteger>>(vm, -1),
            (std::vector<SQInteger>{33, 27, 22, 11}));

  array_ctx.pop();
  ASSERT_EQ(squip::unpack<std::vector<SQInteger>>(vm, -1),
            (std::vector<SQInteger>{33, 27, 22}));

  array_ctx.remove(1);
  ASSERT_EQ(squip::unpack<std::vector<SQInteger>>(vm, -1),
            (std::vector<SQInteger>{33, 22}));

  array_ctx.clear();
  ASSERT_EQ(array_ctx.size(), 0);
  ASSERT_EQ(squip::unpack<std::vector<SQInteger>>(vm, -1),
            (std::vector<SQInteger>{}));
}

/* EOF */
