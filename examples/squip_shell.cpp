#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <squirrel.h>
#include <sqstdaux.h>

#include <squip/squip.hpp>

namespace {

struct Options
{
  bool debug = false;
  std::vector<std::string> files = {};
  std::vector<std::string> rest = {};
};

Options parse_args(int argc, char** argv)
{
  Options opts;
  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] == '-')
    {
      if (strcmp(argv[i], "-f") == 0 ||
          strcmp(argv[i], "--file") == 0)
      {
        i += 1;
        if (i >= argc) {
          throw std::runtime_error(fmt::format("'{}' requires an argument", argv[i - 1]));
        }

        opts.files.emplace_back(argv[i]);
      }
      else if (strcmp(argv[i], "-d") == 0 ||
               strcmp(argv[i], "--debug") == 0)
      {
        opts.debug = true;
      }
      else if (strcmp(argv[i], "-D") == 0 ||
               strcmp(argv[i], "--no-debug") == 0)
      {
        opts.debug = false;
      }
      else
      {
        throw std::runtime_error(fmt::format("unknown option: '{}'", argv[i]));
      }
    }
    else // rest arguments
    {
      opts.rest.emplace_back(argv[i]);
    }
  }

  return opts;
}

} // namespace

int main(int argc, char** argv) try
{
  Options const opts = parse_args(argc, argv);

  squip::SquirrelVM sqvm;

  sqvm.set_printfunc(
    [](char const* msg){
      std::cout << msg;
    },
    [](char const* msg){
      std::cerr << msg;
    });

  //sqstd_seterrorhandlers(sqvm.get_vm());
  sqvm.set_errorhandler([](HSQUIRRELVM vm) {
    if (sq_gettop(vm) == 2) {
      SQChar const* msg;
      if (SQ_SUCCEEDED(sq_getstring(vm, 2, &msg))) {
        // regular error, aka 'throw "string"'
        squip::print_stacktrace(vm, std::cerr);
        std::cerr << "error: " << msg << std::endl;
      } else {
        if ((false)) {
          // non-string exception
          squip::print_stacktrace(vm, std::cerr);
          std::cerr << "error: <unknown failure>" << std::endl;
        }
      }
    } else {
      // weird stack state, shouldn't happen
      squip::print_stacktrace(vm, std::cerr);
      assert(false && "UNKNOWN FAILURE MODE");
    }
  });

  sqvm.set_compilererrorhandler([](char const* desc, char const* source, SQInteger line, SQInteger column){
    std::cerr << (source ? source : "<null>") << ":"
              << line << ":" << column << ": error: "
              << (desc ? desc : "<null>")
              << std::endl;
  });

  if (opts.debug)
  {
    sq_enabledebuginfo(sqvm.get_vm(), opts.debug);
    sq_notifyallexceptions(sqvm.get_vm(), SQTrue);
    sq_setnativedebughook(
      sqvm.get_vm(),
      [](HSQUIRRELVM vm, SQInteger event_type, SQChar const* sourcename, SQInteger line, SQChar const* funcname)
      {
        std::cout << "[DBG] ";
        switch (event_type)
        {
          case 'l':
            std::cout << "line: ";
            break;

          case 'c':
            std::cout << "call: ";
            break;

          case 'r':
            std::cout << "return: ";
            break;

          default:
            assert(false && "never reached");
        }

        std::cout << ": " << (sourcename ? sourcename : "<null>")
                  << ":" << line
                  << ":" << (funcname ? funcname : "<null>") << "\n";
      });
  }

  sqvm.bind("doit", ". i|f|b i|f|b i|f|b", [](HSQUIRRELVM vm) -> SQInteger {
    std::cout << "custom function: enter\n";

    SQInteger top = -4;
    SQInteger arg1;
    if(SQ_FAILED(sq_getinteger(vm, top + 0, &arg1))) {
      sq_throwerror(vm, _SC("Argument 1 not an integer"));
      return SQ_ERROR;
    }

    SQInteger arg2;
    if(SQ_FAILED(sq_getinteger(vm, top + 1, &arg2))) {
      sq_throwerror(vm, _SC("Argument 2 not an integer"));
      return SQ_ERROR;
    }

    SQInteger arg3;
    if(SQ_FAILED(sq_getinteger(vm, top + 2, &arg3))) {
      sq_throwerror(vm, _SC("Argument 3 not an integer"));
      return SQ_ERROR;
    }

    SQUserPointer userptr;
    if(SQ_FAILED(sq_getuserpointer(vm, top + 3, &userptr))) {
      sq_throwerror(vm, _SC("Argument 4 not a userpointer"));
      return SQ_ERROR;
    }

    std::cout << "arguments: " << arg1 << " " << arg2 << " " << arg3 << std::endl;
    std::cout << "custom function: end\n";

    return SQ_OK;
  });

  sqvm.bindpp("dodo", ".", [](HSQUIRRELVM vm) -> SQInteger {
    std::cout << "dodo!" << std::endl;
    return SQ_OK;
  });

  sqvm.bind("myprintln", "..", [](HSQUIRRELVM vm) -> SQInteger {
    squip::print(vm, -1, std::cout);
    std::cout << std::endl;
    return SQ_OK;
  });

  // should result in nothing
  squip::print_stack(sqvm.get_vm(), std::cerr);

  for (auto const& filename : opts.files) {
    std::ifstream fin(filename);
    if (!fin) {
      throw std::runtime_error(fmt::format("failed to load: {}", filename));
    }

    squip::compile_and_run(sqvm.get_vm(), fin, filename);
  }

  for (auto const& text : opts.rest) {
    std::istringstream is(text);
    squip::compile_and_run(sqvm.get_vm(), is, "<source>");
  }

  // should result in nothing too, otherwise stack is corrupted
  squip::print_stack(sqvm.get_vm(), std::cerr);

  if (sq_gettop(sqvm.get_vm()) == 1) {
    throw std::runtime_error("stack corrupted");
  }

  return EXIT_SUCCESS;
}
catch (std::exception const& err) {
  std::cerr << "error: " << err.what() << std::endl;
  return EXIT_FAILURE;
}

/* EOF */

