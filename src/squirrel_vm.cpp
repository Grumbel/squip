//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "squip/squirrel_vm.hpp"

#include <cstdarg>
#include <cstring>
#include <stdexcept>

#include "squip/squirrel_error.hpp"
#include "squip/squirrel_util.hpp"

namespace squip {

namespace {

inline
char const* format_to_scratchpad(HSQUIRRELVM vm, char const* fmt, va_list args)
{
  if (strcmp(fmt, "%s") == 0)
  {
    // Squirrel just uses "%s"
    char const* text = va_arg(args, char const*);

    return text;
  }
  else
  {
    // only some parts of the Squirrel stdlib need full fmt support
    size_t len = 1024;
    SQChar* text = sq_getscratchpad(vm, len);

    // vsnprintf() will invalidate args
    va_list args2;
    va_copy(args2, args);

    size_t ret = vsnprintf(text, len, fmt, args);
    if (ret >= len) {  // buffer to small, resize, try again
      len = ret + 1;  // one more for trailing '\0'
      text = sq_getscratchpad(vm, len);
      ret = vsnprintf(text, len, fmt, args2);
      va_end(args2);
      assert(ret < len);
    }

    return text;
  }
}

} // namespace

#ifdef __clang__
__attribute__((__format__ (__printf__, 2, 0)))
#endif
void
SquirrelVM::my_printfunc(HSQUIRRELVM vm, char const* fmt, ...)
{
  SquirrelVM* const sqvm = reinterpret_cast<SquirrelVM*>(sq_getforeignptr(vm));

  va_list args;
  va_start(args, fmt);
  char const* text = format_to_scratchpad(vm, fmt, args);
  va_end(args);

  if (sqvm->m_printfunc) {
    sqvm->m_printfunc(text);
  }
}

#ifdef __clang__
__attribute__((__format__ (__printf__, 2, 0)))
#endif
void
SquirrelVM::my_errorfunc(HSQUIRRELVM vm, char const* fmt, ...)
{
  SquirrelVM * const sqvm = reinterpret_cast<SquirrelVM*>(sq_getforeignptr(vm));

  va_list args;
  va_start(args, fmt);
  char const* text = format_to_scratchpad(vm, fmt, args);
  va_end(args);

  if (sqvm->m_errorfunc) {
    sqvm->m_errorfunc(text);
  }
}

void
SquirrelVM::my_compilererrorhandler(HSQUIRRELVM vm,
                                    SQChar const* desc, SQChar const* source, SQInteger line, SQInteger column)
{
  SquirrelVM* sqvm = reinterpret_cast<SquirrelVM*>(sq_getforeignptr(vm));
  assert(sqvm != nullptr);

  if (sqvm->m_compilererrorhandler) {
    sqvm->m_compilererrorhandler(desc, source, line, column);
  }
}

SQRESULT
SquirrelVM::my_errorhandler(HSQUIRRELVM vm)
{
  SquirrelVM* sqvm = reinterpret_cast<SquirrelVM*>(sq_getforeignptr(vm));
  assert(sqvm != nullptr);

  if (sqvm->m_errorhandler) {
    sqvm->m_errorhandler(vm);
  }

  return SQ_OK;
}

SquirrelVM::SquirrelVM() :
  m_vm(),
  m_printfunc(),
  m_errorfunc(),
  m_compilererrorhandler(),
  m_errorhandler()
{
  m_vm = sq_open(64);
  if (m_vm == nullptr) {
    throw std::runtime_error("Couldn't initialize squirrel vm");
  }

  sq_setforeignptr(m_vm, this);
}

SquirrelVM::~SquirrelVM()
{
#ifdef ENABLE_SQDBG
  if (debugger != nullptr) {
    sq_rdbg_shutdown(debugger);
    debugger = nullptr;
  }
#endif

  sq_close(m_vm);
}

void
SquirrelVM::set_printfunc(std::function<void (char const*)> printfunc,
                          std::function<void (char const*)> errorfunc)
{
  m_printfunc = std::move(printfunc);
  m_errorfunc = std::move(errorfunc);
  sq_setprintfunc(m_vm, &SquirrelVM::my_printfunc, &SquirrelVM::my_errorfunc);
}

void
SquirrelVM::set_compilererrorhandler(std::function<void (SQChar const*, SQChar const*, SQInteger, SQInteger)> compilererrorhandler)
{
  m_compilererrorhandler = std::move(compilererrorhandler);
  sq_setcompilererrorhandler(m_vm, &SquirrelVM::my_compilererrorhandler);
}

void
SquirrelVM::set_errorhandler(std::function<void (HSQUIRRELVM)> errorhandler)
{
  m_errorhandler = errorhandler;
  sq_newclosure(m_vm, &SquirrelVM::my_errorhandler, 0);
  sq_seterrorhandler(m_vm);
}

void
SquirrelVM::begin_table(const char* name)
{
  sq_pushstring(m_vm, name, -1);
  sq_newtable(m_vm);
}

void
SquirrelVM::end_table(const char* name)
{
  if (SQ_FAILED(sq_createslot(m_vm, -3)))
    throw SquirrelError(m_vm, "Failed to create '" + std::string(name) + "' table entry");
}

void
SquirrelVM::create_empty_table(const char* name)
{
  begin_table(name);
  end_table(name);
}

bool
SquirrelVM::has_property(const char* name)
{
  sq_pushstring(m_vm, name, -1);
  if (SQ_FAILED(sq_get(m_vm, -2))) return false;
  sq_pop(m_vm, 1);
  return true;
}

void
SquirrelVM::store_bool(const char* name, bool val)
{
  sq_pushstring(m_vm, name, -1);
  sq_pushbool(m_vm, val ? SQTrue : SQFalse);
  if (SQ_FAILED(sq_createslot(m_vm, -3)))
    throw SquirrelError(m_vm, "Couldn't add float value to table");
}

void
SquirrelVM::store_int(const char* name, int val)
{
  sq_pushstring(m_vm, name, -1);
  sq_pushinteger(m_vm, val);
  if (SQ_FAILED(sq_createslot(m_vm, -3)))
    throw SquirrelError(m_vm, "Couldn't add int value to table");
}

void
SquirrelVM::store_float(const char* name, float val)
{
  sq_pushstring(m_vm, name, -1);
  sq_pushfloat(m_vm, val);
  if (SQ_FAILED(sq_createslot(m_vm, -3)))
    throw SquirrelError(m_vm, "Couldn't add float value to table");
}

void
SquirrelVM::store_string(const char* name, const std::string& val)
{
  sq_pushstring(m_vm, name, -1);
  sq_pushstring(m_vm, val.c_str(), val.length());
  if (SQ_FAILED(sq_createslot(m_vm, -3)))
    throw SquirrelError(m_vm, "Couldn't add float value to table");
}

void
SquirrelVM::store_object(const char* name, const HSQOBJECT& val)
{
  sq_pushstring(m_vm, name, -1);
  sq_pushobject(m_vm, val);
  if (SQ_FAILED(sq_createslot(m_vm, -3)))
    throw SquirrelError(m_vm, "Couldn't add object value to table");
}

bool
SquirrelVM::get_bool(const char* name, bool& val)
{
  if (!has_property(name)) return false;
  val = read_bool(name);
  return true;
}

bool
SquirrelVM::get_int(const char* name, int& val)
{
  if (!has_property(name)) return false;
  val = read_int(name);
  return true;
}

bool
SquirrelVM::get_float(const char* name, float& val)
{
  if (!has_property(name)) return false;
  val = read_float(name);
  return true;
}

bool
SquirrelVM::get_string(const char* name, std::string& val)
{
  if (!has_property(name)) return false;
  val = read_string(name);
  return true;
}

bool
SquirrelVM::read_bool(const char* name)
{
  get_table_entry(name);

  SQBool result;
  if (SQ_FAILED(sq_getbool(m_vm, -1, &result))) {
    std::ostringstream msg;
    msg << "Couldn't get bool value for '" << name << "' from table";
    throw SquirrelError(m_vm, msg.str());
  }
  sq_pop(m_vm, 1);

  return result == SQTrue;
}

int
SquirrelVM::read_int(const char* name)
{
  get_table_entry(name);

  SQInteger result;
  if (SQ_FAILED(sq_getinteger(m_vm, -1, &result))) {
    std::ostringstream msg;
    msg << "Couldn't get int value for '" << name << "' from table";
    throw SquirrelError(m_vm, msg.str());
  }
  sq_pop(m_vm, 1);

  return static_cast<int>(result);
}

float
SquirrelVM::read_float(const char* name)
{
  get_table_entry(name);

  float result;
  if (SQ_FAILED(sq_getfloat(m_vm, -1, &result))) {
    std::ostringstream msg;
    msg << "Couldn't get float value for '" << name << "' from table";
    throw SquirrelError(m_vm, msg.str());
  }
  sq_pop(m_vm, 1);

  return result;
}

std::string
SquirrelVM::read_string(const char* name)
{
  get_table_entry(name);

  const char* result;
  if (SQ_FAILED(sq_getstring(m_vm, -1, &result))) {
    std::ostringstream msg;
    msg << "Couldn't get string value for '" << name << "' from table";
    throw SquirrelError(m_vm, msg.str());
  }
  sq_pop(m_vm, 1);

  return std::string(result);
}

void
SquirrelVM::get_table_entry(const std::string& name)
{
  sq_pushstring(m_vm, name.c_str(), -1);
  if (SQ_FAILED(sq_get(m_vm, -2)))
  {
    std::ostringstream msg;
    msg << "failed to get '" << name << "' table entry";
    throw SquirrelError(m_vm, msg.str());
  }
  else
  {
    // successfully placed result on stack
  }
}

void
SquirrelVM::get_or_create_table_entry(const std::string& name)
{
  try
  {
    get_table_entry(name);
  }
  catch(std::exception&)
  {
    create_empty_table(name.c_str());
    get_table_entry(name);
  }
}

void
SquirrelVM::delete_table_entry(const char* name)
{
  sq_pushstring(m_vm, name, -1);
  if (SQ_FAILED(sq_deleteslot(m_vm, -2, false)))
  {
    // Something failed while deleting the table entry.
    // Key doesn't exist?
  }
}

void
SquirrelVM::rename_table_entry(const char* oldname, const char* newname)
{
  SQInteger oldtop = sq_gettop(m_vm);

  // push key
  sq_pushstring(m_vm, newname, -1);

  // push value and delete old oldname
  sq_pushstring(m_vm, oldname, -1);
  if (SQ_FAILED(sq_deleteslot(m_vm, oldtop, SQTrue))) {
    sq_settop(m_vm, oldtop);
    throw SquirrelError(m_vm, "Couldn't find 'oldname' entry in table");
  }

  // create new entry
  sq_createslot(m_vm, -3);

  sq_settop(m_vm, oldtop);
}

std::vector<std::string>
SquirrelVM::get_table_keys()
{
  auto old_top = sq_gettop(m_vm);
  std::vector<std::string> keys;

  sq_pushnull(m_vm);
  while (SQ_SUCCEEDED(sq_next(m_vm, -2)))
  {
    //here -1 is the value and -2 is the key
    const char* result;
    if (SQ_FAILED(sq_getstring(m_vm, -2, &result)))
    {
      throw SquirrelError(m_vm, "Couldn't get string value for key");
    }
    else
    {
      keys.push_back(result);
    }

    // pops key and val before the next iteration
    sq_pop(m_vm, 2);
  }

  sq_settop(m_vm, old_top);

  return keys;
}

void
SquirrelVM::bind(char const* name, char const* typemask, SQFUNCTION func)
{
  sq_pushroottable(m_vm);
  sq_pushstring(m_vm, name, -1);
  sq_newclosure(m_vm, func, 0);
  sq_setparamscheck(m_vm, SQ_MATCHTYPEMASKSTRING, typemask);

  if(SQ_FAILED(sq_createslot(m_vm, -3))) {
    throw SquirrelError(m_vm, "Couldn't register function");
  }

  sq_pop(m_vm, 1);
}

void
SquirrelVM::bindpp(const char* name, const char* typemask, std::function<SQInteger (HSQUIRRELVM)> func)
{
  sq_pushroottable(m_vm);
  sq_pushstring(m_vm, name, -1);

  SQUserPointer userptr = sq_newuserdata(m_vm, sizeof(func));
  new(userptr) std::function<SQInteger (HSQUIRRELVM)>(std::move(func));
  sq_setreleasehook(m_vm, -1, [](SQUserPointer uptr, SQInteger size) -> SQInteger {
    auto* funcptr = reinterpret_cast<std::function<SQInteger (HSQUIRRELVM)>*>(uptr);
    funcptr->~function<SQInteger (HSQUIRRELVM)>();
    return 1;
  });

  sq_newclosure(m_vm, [](HSQUIRRELVM vm) -> SQInteger {
    SQUserPointer uptr;
    if (SQ_FAILED(sq_getuserdata(vm, -1, &uptr, nullptr))) {
      sq_throwerror(vm, "invalid argument, must be userdata");
      return SQ_ERROR;
    }
    auto* funcptr = reinterpret_cast<std::function<SQInteger (HSQUIRRELVM)>*>(uptr);
    try {
      return (*funcptr)(vm);
    } catch (std::exception const& err) {
      return sq_throwerror(vm, err.what());
    }
  }, 1);
  sq_setparamscheck(m_vm, SQ_MATCHTYPEMASKSTRING, typemask);

  if (SQ_FAILED(sq_createslot(m_vm, -3))) {
    throw SquirrelError(m_vm, "Couldn't register function");
  }

  sq_pop(m_vm, 1);
}

HSQOBJECT
SquirrelVM::create_thread()
{
  HSQUIRRELVM new_vm = sq_newthread(m_vm, 64);
  if (new_vm == nullptr)
    throw SquirrelError(m_vm, "Couldn't create new VM");

  HSQOBJECT vm_object;
  sq_resetobject(&vm_object);
  if (SQ_FAILED(sq_getstackobj(m_vm, -1, &vm_object)))
    throw SquirrelError(m_vm, "Couldn't get squirrel thread from stack");
  sq_addref(m_vm, &vm_object);

  sq_pop(m_vm, 1);

  return vm_object;
}

} // namespace squip

/* EOF */
