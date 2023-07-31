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

#ifndef HEADER_SQUIP_STACK_CONTEXT_HPP
#define HEADER_SQUIP_STACK_CONTEXT_HPP

#include <squirrel.h>

#include "squip/array_context.hpp"
#include "squip/fwd.hpp"
#include "squip/table_context.hpp"
#include "squip/unpack.hpp"
#include "squip/util.hpp"

namespace squip {

class StackContext
{
public:
  StackContext(HSQUIRRELVM vm);
  ~StackContext();

  StackContext(StackContext const&) = default;
  StackContext& operator=(StackContext const&) = default;

  template<typename T>
  T get(SQInteger idx)
  {
    return squip::unpack<T>(m_vm, idx);
  }

  template<typename T>
  void push(T&& value)
  {
    push_value(m_vm, std::forward<T>(value));
  }

  TableContext push_roottable();

  ArrayContext push_new_array(SQInteger size = 0) {
    return squip::new_array(m_vm, size);
  }

  TableContext push_new_table() {
    return squip::new_table(m_vm);
  }

private:
  HSQUIRRELVM m_vm;
};

} // namespace squip

#endif

/* EOF */
