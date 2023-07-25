//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "squip/squirrel_util.hpp"

#include <cstring>
#include <stdio.h>
#include <sqstdaux.h>
#include <sqstdblob.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <stdarg.h>

#include "squip/script_interface.hpp"

namespace squip {

void print(HSQUIRRELVM vm, SQInteger idx, std::ostream& os)
{
  // convert idx to positive, as sq_pushnull() would otherwise invalidate it
  if (idx < 0) {
    idx = sq_gettop(vm) + idx + 1;
  }

  switch (sq_gettype(vm, idx))
  {
    case OT_NULL:
      os << "null";
      break;

    case OT_BOOL: {
      SQBool p;
      if (SQ_SUCCEEDED(sq_getbool(vm, idx, &p))) {
        if (p)
          os << "true";
        else
          os << "false";
      }
      break;
    }

    case OT_INTEGER: {
      SQInteger val;
      sq_getinteger(vm, idx, &val);
      os << val;
      break;
    }

    case OT_FLOAT: {
      SQFloat val;
      sq_getfloat(vm, idx, &val);
      os << val;
      break;
    }

    case OT_STRING: {
      const SQChar* val;
      sq_getstring(vm, idx, &val);

      os.put('"');
      while (*val) {
        switch (*val) {
          case '\t': os << "\\t"; break;
          case '\a': os << "\\a"; break;
          case '\b': os << "\\b"; break;
          case '\n': os << "\\n"; break;
          case '\r': os << "\\r"; break;
          case '\v': os << "\\v"; break;
          case '\f': os << "\\f"; break;
          case '\0': os << "\\0"; break;
          case '\\': os << "\\\\"; break;
          case '"': os << "\\\""; break;
          default: os << *val; break;
        }
        val += 1;
      }
      os.put('"');
      break;
    }

    case OT_TABLE: {
      bool first = true;
      os << "{";
      sq_pushnull(vm);  //null iterator
      while (SQ_SUCCEEDED(sq_next(vm, idx)))
      {
        if (!first) {
          os << ", ";
        }
        first = false;

        //here -1 is the value and -2 is the key
        os << to_string(vm, -2) << ": "
           << to_string(vm, -1);

        sq_pop(vm, 2); //pops key and val before the nex iteration
      }
      sq_pop(vm, 1);
      os << "}";
      break;
    }

    case OT_ARRAY: {
      bool first = true;
      os << "[";
      sq_pushnull(vm);  //null iterator
      while (SQ_SUCCEEDED(sq_next(vm, idx)))
      {
        if (!first) {
          os << ", ";
        }
        first = false;

        //here -1 is the value and -2 is the key
        // we ignore the key, since that is just the index in an array
        os << to_string(vm, -1);

        sq_pop(vm, 2); //pops key and val before the nex iteration
      }
      sq_pop(vm, 1);
      os << "]";
      break;
    }

    case OT_USERDATA:
      os << "<userdata>";
      break;

    case OT_CLOSURE:
      os << "<closure>";
      break;

    case OT_NATIVECLOSURE:
      os << "<native closure>";
      break;

    case OT_GENERATOR:
      os << "<generator>";
      break;

    case OT_USERPOINTER:
      os << "userpointer";
      break;

    case OT_THREAD:
      os << "<thread>";
      break;

    case OT_CLASS:
      os << "<class>";
      break;

    case OT_INSTANCE:
      os << "<instance>";
      break;

    case OT_WEAKREF:
      os << "<weakref>";
      break;

    default:
      os << "<unknown>";
      break;
  }
}

std::string to_string(HSQUIRRELVM vm, SQInteger idx)
{
  std::ostringstream os;
  print(vm, idx, os);
  return os.str();
}

void print_stack(HSQUIRRELVM vm, std::ostream& os)
{
  SQInteger const top = sq_gettop(vm);

  for (int i = 1; i <= top; ++i)
  {
    os << i << ": ";
    print(vm, i, os);
    os << std::endl;
  }
}

void print_stacktrace(HSQUIRRELVM vm, std::ostream& os)
{
  SQStackInfos stackinfos;

  SQInteger max_level = 1;
  while (SQ_SUCCEEDED(sq_stackinfos(vm, max_level++, &stackinfos))) {}

  for (SQInteger level = max_level - 2; level > 0; --level)
  {
    SQInteger result = sq_stackinfos(vm, level, &stackinfos);
    assert(SQ_SUCCEEDED(result));

    SQChar const* source = stackinfos.source ? stackinfos.source : "<unknown>";
    SQChar const* funcname = stackinfos.funcname ? stackinfos.funcname : "<unknown>";

    os << "#" << (max_level - level - 2) << "  " << stackinfos.funcname << "(" << ")\n"
       << "  at " << source << ":" << stackinfos.line << std::endl;

    // FIXME: Any way to find out which of those are function
    // arguments and which are variables? sq_getclosureinfo() needs a closure
    SQInteger seq = 0;
    SQChar const* name = nullptr;
    while((name = sq_getlocal(vm, level, seq++)) != nullptr)
    {
      os << "  ";
      // skip 'this' to reduce the noise
      if (std::strcmp(name, "this") == 0) {
        os << "this = <table>";
      } else {
        os << name;
        os << " = ";
        print(vm, -1, os);
      }
      os << std::endl;

      sq_pop(vm, 1);
    }
  }
}

SQInteger squirrel_read_char(SQUserPointer file)
{
  std::istream* in = reinterpret_cast<std::istream*> (file);
  int c = in->get();
  if (in->eof())
    return 0;
  return c;
}

void compile_script(HSQUIRRELVM vm, std::istream& in, const std::string& sourcename)
{
  if (SQ_FAILED(sq_compile(vm, squirrel_read_char, &in, sourcename.c_str(), SQTrue))) {
    throw SquirrelError(vm, "failed to compile script");
  }
}

void compile_and_run(HSQUIRRELVM vm, std::istream& in,
                     const std::string& sourcename)
{
  compile_script(vm, in, sourcename);

  SQInteger oldtop = sq_gettop(vm);

  try {
    sq_pushroottable(vm);
    if (SQ_FAILED(sq_call(vm, 1, SQFalse, SQTrue)))
      throw SquirrelError(vm, "Couldn't start script");
  } catch(...) {
    sq_settop(vm, oldtop);
    throw;
  }

  // we can remove the closure in case the script was not suspended
  if (sq_getvmstate(vm) != SQ_VMSTATE_SUSPENDED) {
    sq_settop(vm, oldtop-1);
  }
}

HSQUIRRELVM object_to_vm(HSQOBJECT object)
{
  if (object._type != OT_THREAD)
    return nullptr;

  return object._unVal.pThread;
}

} // namespace squip

/* EOF */
