#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include <squip/squirrel_vm.hpp>
#include <squip/util.hpp>

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

TEST(SquipSquirrelVM, thread)
{
  squip::SquirrelVM sqvm;
  std::ostringstream os;
  sqvm.set_printfunc([&os](char const* msg){ os << msg; },
                     [&os](char const* msg){ os << msg; });

  std::istringstream is("g_counter <- 0");
  squip::compile_and_run(sqvm.get_vm(), is, "<source>");

  std::vector<squip::Thread> threads;
  threads.emplace_back(sqvm.create_thread());
  threads.emplace_back(sqvm.create_thread());

  threads[0].run_script("tests/thread1.nut");
  threads[1].run_script("tests/thread2.nut");

  bool done = false;
  while (!done) {
    done = true;
    for (auto& thread : threads) {
      if (thread.is_suspended()) {
        thread.wakeup();
        done = false;
      }
    }
  }

  ASSERT_EQ(os.str(),
            "Hello, World from thread number one!\n"
            "0 --- Thread1: 0\n"
            "Hello, World from thread number two!\n"
            "1 +++ Thread2: 0\n"
            "2 --- Thread1: 1\n"
            "3 +++ Thread2: 1\n"
            "4 --- Thread1: 2\n"
            "5 +++ Thread2: 2\n"
            "6 --- Thread1: 3\n"
            "7 +++ Thread2: 3\n"
            "8 --- Thread1: 4\n"
            "9 +++ Thread2: 4\n"
            "10 --- Thread1: 5\n"
            "11 +++ Thread2: 5\n"
            "12 --- Thread1: 6\n"
            "13 +++ Thread2: 6\n"
            "14 --- Thread1: 7\n"
            "15 +++ Thread2: 7\n"
            "16 --- Thread1: 8\n"
            "17 +++ Thread2: 8\n"
            "18 --- Thread1: 9\n"
            "19 +++ Thread2: 9\n");
}

/* EOF */
