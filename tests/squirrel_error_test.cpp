#include <gtest/gtest.h>

#include <squip/squirrel_util.hpp>
#include <squip/squirrel_vm.hpp>

TEST(SquipSquirrelError, error)
{
  squip::SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();
  std::istringstream is;

  EXPECT_THROW({
      throw squip::SquirrelError(vm, "test error");
    }, squip::SquirrelError);

  try {
    throw squip::SquirrelError(vm, "TestError");
  } catch(squip::SquirrelError const& err) {
    EXPECT_STREQ(err.what(), "SquirrelError: TestError (null)");
  }

  try {
    is = std::istringstream("class TestErrorFromSquirrel { function _tostring() { return \"TestString\"; } }; "
                            "throw TestErrorFromSquirrel();");
    squip::compile_and_run(vm, is, "<source>");
    FAIL();
  } catch(squip::SquirrelError const& err) {
    EXPECT_STREQ(err.what(), "SquirrelError: Couldn't start script (TestString)");
  }

  is = std::istringstream("a + b");
  EXPECT_THROW({
      squip::compile_and_run(vm, is, "<source>");
    }, squip::SquirrelError);

  is = std::istringstream("11 + 22");
  EXPECT_NO_THROW({
      squip::compile_and_run(vm, is, "<source>");
    });
}

/* EOF */

