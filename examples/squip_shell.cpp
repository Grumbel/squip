#include <stdexcept>
#include <iostream>

#include <squip/squip.hpp>

int main(int argc, char** argv)
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

  sqvm.bind("doit", ". i|f|b i|f|b i|f|b", [](HSQUIRRELVM vm) -> SQInteger {
    std::cout << "custom function: enter\n";

    SQInteger arg1;
    if(SQ_FAILED(sq_getinteger(vm, -3, &arg1))) {
      sq_throwerror(vm, _SC("Argument 1 not an integer"));
      return SQ_ERROR;
    }

    SQInteger arg2;
    if(SQ_FAILED(sq_getinteger(vm, -2, &arg2))) {
      sq_throwerror(vm, _SC("Argument 2 not an integer"));
      return SQ_ERROR;
    }

    SQInteger arg3;
    if(SQ_FAILED(sq_getinteger(vm, -1, &arg3))) {
      sq_throwerror(vm, _SC("Argument 3 not an integer"));
      return SQ_ERROR;
    }

    squip::print_stack(vm, std::cout);

    std::cout << "arguments: " << arg1 << " " << arg2 << " " << arg3 << std::endl;
    std::cout << "custom function: end\n";

    return SQ_OK;
  });

  // should result in nothing
  squip::print_stack(sqvm.get_vm(), std::cerr);

  for (int i = 1; i < argc; ++i)
  {
    std::istringstream is(argv[i]);
    squip::compile_and_run(sqvm.get_vm(), is, "<source>");
  }

  // should result in nothing too, otherwise stack is corrupted
  squip::print_stack(sqvm.get_vm(), std::cerr);

  if (sq_gettop(sqvm.get_vm()) == 1) {
    throw std::runtime_error("stack corrupted");
  }

  return EXIT_SUCCESS;
}

/* EOF */

