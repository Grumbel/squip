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

TableContext
SquirrelVM::get_roottable() const
{
  sq_pushroottable(m_vm);
  return TableContext(m_vm);
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
