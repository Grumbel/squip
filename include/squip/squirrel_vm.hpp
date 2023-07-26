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

#ifndef HEADER_SUPERTUX_SQUIRREL_SQUIRREL_VM_HPP
#define HEADER_SUPERTUX_SQUIRREL_SQUIRREL_VM_HPP

#include <functional>
#include <string>
#include <vector>

#include <squirrel.h>

#include "squip/squirrel_error.hpp"
#include "squip/table_context.hpp"

namespace squip {

/** Basic wrapper around HSQUIRRELVM with some utility functions, not
    to be confused with SquirrelVirtualMachine. The classes might be
    merged in the future. */
class SquirrelVM
{
public:
  SquirrelVM();
  ~SquirrelVM();

  HSQUIRRELVM get_vm() const { return m_vm; }

  void set_printfunc(std::function<void (char const*)> printfunc,
                     std::function<void (char const*)> errorfunc);
  void set_compilererrorhandler(std::function<
                                void (SQChar const*, SQChar const*, SQInteger, SQInteger)> compilererrorhandler);
  void set_errorhandler(std::function<void (HSQUIRRELVM)> errorhandler);

  TableContext get_roottable() const;

  HSQOBJECT create_thread();

private:
  static void my_printfunc(HSQUIRRELVM vm, const char* fmt, ...);
  static void my_errorfunc(HSQUIRRELVM vm, const char* fmt, ...);
  static void my_compilererrorhandler(HSQUIRRELVM vm, SQChar const* desc, SQChar const* source, SQInteger line, SQInteger column);
  static SQRESULT my_errorhandler(HSQUIRRELVM vm);

private:
  HSQUIRRELVM m_vm;

  std::function<void (char const*)> m_printfunc;
  std::function<void (char const*)> m_errorfunc;
  std::function<void (SQChar const*, SQChar const*, SQInteger, SQInteger)> m_compilererrorhandler;
  std::function<void (HSQUIRRELVM)> m_errorhandler;

private:
  SquirrelVM(const SquirrelVM&) = delete;
  SquirrelVM& operator=(const SquirrelVM&) = delete;
};

} // namespace squip

#endif

/* EOF */
