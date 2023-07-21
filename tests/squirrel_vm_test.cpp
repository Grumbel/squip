#include <gtest/gtest.h>

#include <sstream>

#include <squip/squirrel_util.hpp>
#include <squip/squirrel_vm.hpp>

TEST(SquipSquirrelVM, print)
{
  squip::SquirrelVM sqvm;

  sqvm.set_printfunc(
    [](char const* msg){
      std::cout << msg;
    },
    [](char const* msg){
      std::cerr << msg;
    });

  sqvm.set_compilererrorhandler([](char const* desc, char const* source, SQInteger line, SQInteger column){
    std::cerr << (source ? source : "<null>") << ":"
              << line << ":" << column << ": "
              << (desc ? desc : "<null>")
              << std::endl;
  });

  char const* source =
    "local a = 5\n"
    "local b = 10\n"
    "print(a + b // missing an ')' to trigger an error\n";

  std::istringstream is(source);
  EXPECT_THROW(squip::compile_and_run(sqvm.get_vm(), is, "<source>"),
               squip::SquirrelError);
}

/* EOF */
