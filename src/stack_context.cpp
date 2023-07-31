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

#include "squip/stack_context.hpp"

#include "squip/table_context.hpp"

namespace squip {

StackContext::StackContext(HSQUIRRELVM vm) :
  m_vm(vm)
{
}

StackContext::~StackContext()
{
}

TableContext
StackContext::push_roottable()
{
  sq_pushroottable(m_vm);
  return TableContext(m_vm, -1);
}

} // namespace squip

/* EOF */
