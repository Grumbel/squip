// squip - Utilities for Squirrel
// Copyright (C) 2023 Ingo Ruhnke <grumbel@gmail.com>
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

#include "squip/array_context.hpp"

#include <cassert>

#include "squip/squirrel_error.hpp"

namespace squip {

ArrayContext::ArrayContext(HSQUIRRELVM vm, SQInteger idx) :
  m_vm(vm),
  m_idx(absolute_index(m_vm, idx))
{
  assert(sq_gettype(m_vm, m_idx) == OT_ARRAY);
}

ArrayContext::~ArrayContext()
{}

void
ArrayContext::append()
{
  if (SQ_FAILED(sq_arrayappend(m_vm, m_idx))) {
    throw SquirrelError::from_vm(m_vm, "failed to append item to array");
  }
}

void
ArrayContext::insert(SQInteger destpos)
{
  if (SQ_FAILED(sq_arrayinsert(m_vm, m_idx, destpos))) {
    throw SquirrelError::from_vm(m_vm, "failed to insert item into array");
  }
}

void
ArrayContext::pop(SQBool pushval)
{
  if (SQ_FAILED(sq_arraypop(m_vm, m_idx, pushval))) {
    throw SquirrelError::from_vm(m_vm, "failed to pop item from array");
  }
}

void
ArrayContext::remove(SQInteger itemidx)
{
  if (SQ_FAILED(sq_arrayremove(m_vm, m_idx, itemidx))) {
    throw SquirrelError::from_vm(m_vm, "failed to remove item from array");
  }
}

void
ArrayContext::resize(SQInteger size)
{
  if (SQ_FAILED(sq_arrayresize(m_vm, m_idx, size))) {
    throw SquirrelError::from_vm(m_vm, "failed to resize array");
  }
}

void
ArrayContext::reverse()
{
  if (SQ_FAILED(sq_arrayreverse(m_vm, m_idx))) {
    throw SquirrelError::from_vm(m_vm, "failed to reverse array");
  }
}

void
ArrayContext::clear()
{
  if (SQ_FAILED(sq_clear(m_vm, m_idx))) {
    throw SquirrelError::from_vm(m_vm, "failed to clear array");
  }
}

SQInteger
ArrayContext::size()
{
  return sq_getsize(m_vm, m_idx);
}

} // namespace squip

/* EOF */
