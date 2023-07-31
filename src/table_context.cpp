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

#include "squip/table_context.hpp"

#include <cassert>
#include <sstream>
#include <fmt/format.h>

#include "squip/squirrel_error.hpp"
#include "squip/util.hpp"

namespace squip {

TableContext::TableContext(HSQUIRRELVM vm, SQInteger idx) :
  m_vm(vm),
  m_idx(absolute_index(vm, idx))
{
  assert(sq_gettype(m_vm, m_idx) == OT_TABLE);
}

TableContext::~TableContext()
{
}

bool
TableContext::has_key(std::string_view name)
{
  sq_pushstring(m_vm, name.data(), name.size());
  if (SQ_FAILED(sq_get(m_vm, m_idx))) {
    return false;
  }
  sq_pop(m_vm, 1);
  return true;
}

void
TableContext::store_c_function(std::string_view name, char const* typemask, SQFUNCTION func)
{
  sq_pushstring(m_vm, name.data(), name.size());
  sq_newclosure(m_vm, func, 0);
  sq_setnativeclosurename(m_vm, -1, std::string(name).c_str());
  sq_setparamscheck(m_vm, SQ_MATCHTYPEMASKSTRING, typemask);

  if(SQ_FAILED(sq_createslot(m_vm, m_idx))) {
    throw SquirrelError::from_vm(m_vm, "failed to register function");
  }
}

void
TableContext::store_function(std::string_view name, const char* typemask, std::function<SQInteger (HSQUIRRELVM)> func)
{
  sq_pushstring(m_vm, name.data(), name.size());
  push_function(m_vm, std::move(func));
  sq_setnativeclosurename(m_vm, -1, std::string(name).c_str());
  sq_setparamscheck(m_vm, SQ_MATCHTYPEMASKSTRING, typemask);

  if (SQ_FAILED(sq_createslot(m_vm, m_idx))) {
    throw SquirrelError::from_vm(m_vm, "failed to register function");
  }
}

void
TableContext::get_entry(std::string_view name)
{
  sq_pushstring(m_vm, name.data(), name.size());
  if (SQ_FAILED(sq_get(m_vm, m_idx)))
  {
    throw SquirrelError::from_vm(m_vm, fmt::format("failed to get '{}' table entry", name));
  }
  else
  {
    // successfully placed result on stack
  }
}

void
TableContext::delete_entry(std::string_view name)
{
  sq_pushstring(m_vm, name.data(), name.size());
  if (SQ_FAILED(sq_deleteslot(m_vm, m_idx, false)))
  {
    // Something failed while deleting the table entry.
    // Key doesn't exist?
  }
}

void
TableContext::rename_entry(std::string_view oldname, std::string_view newname)
{
  SQInteger const oldtop = sq_gettop(m_vm);

  // push key
  sq_pushstring(m_vm, newname.data(), newname.size());

  // push value and delete old oldname
  sq_pushstring(m_vm, oldname.data(), oldname.size());
  if (SQ_FAILED(sq_deleteslot(m_vm, m_idx, SQTrue))) {
    sq_settop(m_vm, oldtop);
    throw SquirrelError::from_vm(m_vm, "failed to find 'oldname' entry in table");
  }

  // create new entry
  sq_createslot(m_vm, m_idx);

  sq_settop(m_vm, oldtop);
}

std::vector<std::string>
TableContext::get_keys()
{
  SQInteger const old_top = sq_gettop(m_vm);
  std::vector<std::string> keys;

  sq_pushnull(m_vm);
  while (SQ_SUCCEEDED(sq_next(m_vm, m_idx)))
  {
    //here -1 is the value and -2 is the key
    char const* result;
    if (SQ_FAILED(sq_getstring(m_vm, -2, &result)))
    {
      throw SquirrelError::from_vm(m_vm, "failed to get string value for key");
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

TableContext
TableContext::create_table(std::string_view name)
{
  sq_newtable(m_vm);

  sq_pushstring(m_vm, name.data(), name.size());
  sq_push(m_vm, -2);

  if (SQ_FAILED(sq_createslot(m_vm, m_idx))) {
    sq_pop(m_vm, 1);
    throw SquirrelError::from_vm(m_vm, "Failed to create '" + std::string(name) + "' table entry");
  }

  return TableContext(m_vm, -1);
}

TableContext
TableContext::create_or_get_table(std::string_view name)
{
  sq_pushstring(m_vm, name.data(), name.size());
  if (SQ_FAILED(sq_get(m_vm, m_idx))) {
    return create_table(name);
  }

  return TableContext(m_vm, -1);
}

} // namespace squip

/* EOF */

