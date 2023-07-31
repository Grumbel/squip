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

#ifndef HEADER_SQUIP_ARRAY_CONTEXT_HPP
#define HEADER_SQUIP_ARRAY_CONTEXT_HPP

#include <squirrel.h>

#include "squip/util.hpp"

namespace squip {

class ArrayContext
{
public:
  ArrayContext(HSQUIRRELVM vm, SQInteger idx);
  ~ArrayContext();

  ArrayContext(ArrayContext const&) = default;
  ArrayContext& operator=(ArrayContext const&) = default;

  void append();
  template<typename T>
  void append(T value) {
    push_value(m_vm, value);
    append();
  }

  void insert(SQInteger destpos);
  template<typename T>
  void insert(SQInteger destpos, T value) {
    push_value(m_vm, value);
    insert(destpos);
  }

  void pop(SQBool pushval = SQFalse);
  void remove(SQInteger itemidx);
  void clear();

  void resize(SQInteger size);
  void reverse();

  SQInteger size();

private:
  HSQUIRRELVM m_vm;
  SQInteger m_idx;
};

} // namespace squip

#endif
