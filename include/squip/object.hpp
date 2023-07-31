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

#ifndef HEADER_SQUIP_OBJECT_HPP
#define HEADER_SQUIP_OBJECT_HPP

#include <squirrel.h>

namespace squip {

class Object
{
public:
  Object();
  Object(HSQUIRRELVM vm, SQInteger idx);
  ~Object();

  Object(Object const& other);
  Object& operator=(Object const& other);

  Object(Object&& other);
  Object& operator=(Object&& other);

  void push(HSQUIRRELVM vm);
  void release();

  SQObjectType get_type() const { return m_handle._type; }
  SQUnsignedInteger get_refcount() {
    if (m_vm == nullptr) {
      return 0;
    } else {
      return sq_getrefcount(m_vm, &m_handle);
    }
  }

  HSQOBJECT get_handle() const { return m_handle; }

private:
  HSQUIRRELVM m_vm;
  HSQOBJECT m_handle;
};

} // namespace squip

#endif
