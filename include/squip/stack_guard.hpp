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

#ifndef HEADER_STACK_GUARD_HPP
#define HEADER_STACK_GUARD_HPP

#include <squirrel.h>

namespace squip {

// Records the stack index and restores it in the destructor
class StackGuard
{
public:
  StackGuard(HSQUIRRELVM vm) :
    m_vm(vm),
    m_oldtop(sq_gettop(m_vm))
  {
  }

  ~StackGuard()
  {
    sq_settop(m_vm, m_oldtop);
  }

private:
  HSQUIRRELVM m_vm;
  SQInteger m_oldtop;

public:
  StackGuard(StackGuard const&) = delete;
  StackGuard& operator=(StackGuard const&) = delete;
};

} // namespace squip

#endif
