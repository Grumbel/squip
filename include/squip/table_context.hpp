// squip - Utilities for Squirrel
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//               2018-2023 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef HEADER_SQUIP_TABLE_CONTEXT_HPP
#define HEADER_SQUIP_TABLE_CONTEXT_HPP

#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include <squirrel.h>

#include "squip/squirrel_error.hpp"
#include "squip/unpack.hpp"

namespace squip {

/**
   Wrapper around Squirrel's table related functions with some
   additional error checking and exceptions. TableContext expects a
   table to already be present on the top of the VM's stack. The
   destructor or end() will pop said table.

   Care must be taken not polute the stack with other values while the
   TableContext object is alive, such as using mulitple TableContext
   objects at the same time.
*/
class TableContext
{
public:
  TableContext(HSQUIRRELVM vm, SQInteger idx);
  ~TableContext();

  bool has_key(std::string_view name);

  template<typename T>
  void store(std::string_view name, T val)
  {
    sq_pushstring(m_vm, name.data(), name.size());
    push_value(m_vm, val);
    if (SQ_FAILED(sq_createslot(m_vm, m_idx))) {
      throw SquirrelError::from_vm(m_vm, "failed to store value in table");
    }
  }

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
  void store_c_function(std::string_view name, char const* typemask, SQFUNCTION func);
  void store_function(std::string_view name, const char* typemask, std::function<SQInteger (HSQUIRRELVM)> func);

  bool read_bool(std::string_view name, bool& val);
  bool read_int(std::string_view name, int& val);
  bool read_float(std::string_view name, float& val);
  bool read_string(std::string_view name, std::string& val);

  template<typename T>
  T get(std::string_view name)
  {
    get_entry(name);

    T result = squip::unpack<T>(m_vm, -1);
    sq_pop(m_vm, 1);

    return result;
  }

  void get_entry(std::string_view name);
  void delete_entry(std::string_view name);
  void rename_entry(std::string_view oldname, std::string_view newname);
  std::vector<std::string> get_keys();

  TableContext create_table(std::string_view name);
  TableContext create_or_get_table(std::string_view name);

private:
  HSQUIRRELVM m_vm;
  SQInteger m_idx;

public:
  TableContext(TableContext const&) = delete;
  TableContext& operator=(TableContext const&) = delete;
};

} // namespace squip

#endif
