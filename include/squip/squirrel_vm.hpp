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

  void begin_table(const char* name);
  void end_table(const char* name);

  /** Creates an empty table with given name
      @param vm VM to create table on
      @param name Name of the table to create */
  void create_empty_table(const char* name);

  bool has_property(const char* name);

  void store_bool(const char* name, bool val);
  void store_int(const char* name, int val);
  void store_float(const char* name, float val);
  void store_string(const char* name, const std::string& val);
  void store_object(const char* name, const HSQOBJECT& val);

  bool get_bool(const char* name, bool& val);
  bool get_int(const char* name, int& val);
  bool get_float(const char* name, float& val);
  bool get_string(const char* name, std::string& val);

  bool read_bool(const char* name);
  int read_int(const char* name);
  float read_float(const char* name);
  std::string read_string(const char* name);

  void get_table_entry(const std::string& name);
  void get_or_create_table_entry(const std::string& name);
  void delete_table_entry(const char* name);
  void rename_table_entry(const char* oldname, const char* newname);
  std::vector<std::string> get_table_keys();

  /* typemask:
     ‘o’ null
     ‘i’ integer
     ‘f’ float
     ‘n’ integer or float
     ‘s’ string
     ‘t’ table
     ‘a’ array
     ‘u’ userdata
     ‘c’ closure and nativeclosure
     ‘g’ generator
     ‘p’ userpointer
     ‘v’ thread
     ‘x’ instance(class instance)
     ‘y’ class
     ‘b’ bool
     ‘.’ any type.
     '|' used as ‘or’ to accept multiple types
  */
  void bind(char const* name, char const* typemask, SQFUNCTION func);
  void bindpp(const char* name, const char* typemask, std::function<SQInteger (HSQUIRRELVM)> func);

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
