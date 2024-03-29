#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include <fmt/format.h>
#include <squirrel.h>
#include <sqstdaux.h>

#include <squip/squip.hpp>
#include <squip/unpack.hpp>
#include <squip/squirrel_error.hpp>

namespace {

struct Options
{
  bool debug = false;
  std::vector<std::variant<std::filesystem::path, std::string>> code = {};
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

        opts.code.emplace_back(std::filesystem::path(argv[i]));
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
      opts.code.emplace_back(std::string(argv[i]));
    }
  }

  return opts;
}

void register_functions(squip::TableContext& tbl)
{
  tbl.store_c_function("doit", ". i|f|b i|f|b i|f|b", [](HSQUIRRELVM vm) -> SQInteger {
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

  tbl.store_function("dodo", ".", [](HSQUIRRELVM vm) -> SQInteger {
    std::cout << "dodo!" << std::endl;
    return SQ_OK;
  });

  tbl.store_function("make_position", ".nn", [](HSQUIRRELVM vm) -> SQInteger {
    auto [x, y] = squip::unpack_args<float, float>(vm);
    fmt::print("make_position: {}, {}\n", x, y);
    return SQ_OK;
  });

  tbl.store_c_function("myprintln", nullptr, [](HSQUIRRELVM vm) -> SQInteger {
    SQInteger const nargs = sq_gettop(vm);
    for (int idx = 1; idx < nargs; ++idx) {
      squip::print(vm, idx + 1, std::cout);
    }
    std::cout << std::endl;
    return SQ_OK;
  });

  tbl.store_c_function("myprint", nullptr, [](HSQUIRRELVM vm) -> SQInteger {
    SQInteger const nargs = sq_gettop(vm);
    for (int idx = 1; idx < nargs; ++idx) {
      squip::print(vm, idx + 1, std::cout);
    }
    return SQ_OK;
  });

  tbl.store_c_function("pprint", nullptr, [](HSQUIRRELVM vm) -> SQInteger {
    SQInteger const nargs = sq_gettop(vm);
    for (int idx = 1; idx < nargs; ++idx) {
      squip::repr(vm, idx + 1, std::cout, true);
    }
    std::cout << std::endl;
    return SQ_OK;
  });
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

  {
    squip::TableContext root = sqvm.push_roottable();
    register_functions(root);
    sq_poptop(sqvm.get_vm());
  }

  { // build a class
    HSQUIRRELVM vm = sqvm.get_vm();
    sq_pushroottable(vm);

    sq_newclass(vm, 0);

    sq_pushstring(vm, "Position", -1);
    sq_push(vm, 2);
    sq_newslot(vm, 1, false);

    sq_pushstring(vm, "x", -1);
    sq_pushinteger(vm, 11);
    sq_newslot(vm, 2, false);

    sq_pushstring(vm, "y", -1);
    sq_pushinteger(vm, 22);
    sq_newslot(vm, 2, false);

    sq_pushstring(vm, "constructor", -1);
    sq_newclosure(vm, [](HSQUIRRELVM lvm) -> SQRESULT {
      sq_setinstanceup(lvm, 1, new std::string("Hello World"));

      // set x
      sq_pushstring(lvm, "x", -1);
      sq_pushinteger(lvm, 111);
      if (SQ_FAILED(sq_set(lvm, 1))) {
        throw squip::SquirrelError::from_vm(lvm, "failed to set");
      }

      // print
      sq_pushstring(lvm, "print", -1);
      if (SQ_FAILED(sq_get(lvm, 1))) {
        throw squip::SquirrelError::from_vm(lvm, "failed to get print()");
      }
      sq_push(lvm, 1);
      sq_call(lvm, 1, SQFalse, SQFalse);
      sq_poptop(lvm);

      // set y
      sq_pushstring(lvm, "y", -1);
      sq_pushinteger(lvm, 222);
      sq_set(lvm, 1);

      return SQ_OK;
    }, 0);
    sq_newslot(vm, 2, false);

    sq_setreleasehook(vm, 2, [](SQUserPointer userptr, SQInteger size) -> SQInteger {
      std::string* stringptr = static_cast<std::string*>(userptr);
      delete stringptr;
      return 1;
    });

    sq_pushstring(vm, "print", -1);
    sq_newclosure(vm, [](HSQUIRRELVM lvm) -> SQRESULT {
      SQInteger x = 333;
      SQInteger y = 666;

      SQUserPointer userptr;
      sq_getinstanceup(lvm, 1, &userptr, nullptr, SQFalse);

      sq_pushstring(lvm, "x", -1);
      sq_get(lvm, 1);
      sq_getinteger(lvm, -1, &x);

      sq_pushstring(lvm, "y", -1);
      sq_get(lvm, 1);
      sq_getinteger(lvm, -1, &y);

      fmt::print("Position: {} - {}, {}\n",
                 *static_cast<std::string*>(userptr),
                 x, y);
      return SQ_OK;
    }, 0);
    sq_newslot(vm, 2, false);

    {
      HSQMEMBERHANDLE x_memberhandle;
      sq_pushstring(vm, "x", -1);
      if (SQ_FAILED(sq_getmemberhandle(vm, 2, &x_memberhandle))) {
        throw squip::SquirrelError::from_vm(vm, "failed to get member handle");
      }

      SQInteger x;
      if (SQ_FAILED(sq_getbyhandle(vm, 2, &x_memberhandle))) {
        throw squip::SquirrelError::from_vm(vm, "failed to get by member handle");
      }

      if (SQ_FAILED(sq_getinteger(vm, -1, &x))) {
        throw squip::SquirrelError::from_vm(vm, "failed to get integer");
      }

      fmt::print("x from member handle: {}\n", x);
      sq_poptop(vm);
    }

    sq_pop(vm, 2);
    assert(sq_gettop(vm) == 0);
  }

  for (auto const& code : opts.code){
    if (std::holds_alternative<std::filesystem::path>(code)) {
      std::filesystem::path const& filename = std::get<std::filesystem::path>(code);
      std::ifstream fin(filename);
      if (!fin) {
        throw std::runtime_error(fmt::format("failed to load: {}", filename.string()));
      }
      squip::compile_and_run(sqvm.get_vm(), fin, filename);
    } else {
      std::string const& text = std::get<std::string>(code);
      std::istringstream is(text);
      squip::compile_and_run(sqvm.get_vm(), is, "<source>");
    }
  }

  if (sq_gettop(sqvm.get_vm()) != 0) {
    std::cerr << "Stack corruption detected:\n";
    squip::print_stack(sqvm.get_vm(), std::cerr);

    throw std::runtime_error("stack corrupted: ");
  }

  return EXIT_SUCCESS;
}
catch (std::exception const& err) {
  std::cerr << "error: " << err.what() << std::endl;
  return EXIT_FAILURE;
}

/* EOF */

