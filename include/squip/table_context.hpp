//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2018-2023 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef TABLE_CTX_HPP
#define TABLE_CTX_HPP

#include <string>
#include <vector>

#include <squirrel.h>

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
  TableContext(HSQUIRRELVM vm);
  ~TableContext();

  TableContext(TableContext&& other);
  TableContext& operator=(TableContext&& other);

  void end();

  bool has_property(const char* name);

  void store_bool(const char* name, bool val);
  void store_int(const char* name, int val);
  void store_float(const char* name, float val);
  void store_string(const char* name, std::string const& val);
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

  TableContext create_table(char const* name);

private:
  HSQUIRRELVM m_vm;

public:
  TableContext(TableContext const&) = delete;
  TableContext& operator=(TableContext const&) = delete;
};

} // namespace squip

#endif
