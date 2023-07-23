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

TEST(SquipSquirrelVM, printfunc)
{
  squip::SquirrelVM sqvm;

  std::ostringstream os_out;
  std::ostringstream os_err;
  sqvm.set_printfunc(
    [&os_out](char const* msg){
      os_out << msg;
    },
    [&os_err](char const* msg){
      os_err << msg;
    });

  auto pf_test = [](HSQUIRRELVM vm, SQPRINTFUNCTION pf, std::ostringstream& os) {
    os.str("");
    pf(vm, "%s", "Testomat");
    EXPECT_EQ(os.str(), "Testomat");

    os.str("");
    pf(vm, "%d, %d\n", 123, 456);
    EXPECT_EQ(os.str(), "123, 456\n");

    for (int j = 0; j < 128; ++j) {
      std::string longtext;
      for (int i = 0; i < j; ++i) {
        longtext += "0123456789abcdef";
      }
      os.str("");
      pf(vm, (longtext + "%d, %d" + longtext).c_str(), 123, 456);
      EXPECT_EQ(os.str(), longtext + "123, 456" + longtext);
    }
  };

  pf_test(sqvm.get_vm(), sq_getprintfunc(sqvm.get_vm()), os_out);
  pf_test(sqvm.get_vm(), sq_geterrorfunc(sqvm.get_vm()), os_err);
}

/* EOF */
