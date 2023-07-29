#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include <fmt/format.h>

#include <squip/stack_guard.hpp>
#include <squip/squirrel_vm.hpp>
#include <squip/util.hpp>

TEST(SquipStackGuard, test)
{
  squip::SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();

  try {
    squip::StackGuard stackguard(vm);
    sq_pushinteger(vm, 11);
    sq_pushinteger(vm, 22);
    sq_pushinteger(vm, 33);
    throw std::runtime_error("stackcrasher");
  } catch (...) {
  }

  ASSERT_EQ(sq_gettop(sqvm.get_vm()), 0);
}

/* EOF */
