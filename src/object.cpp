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

#include "squip/object.hpp"

#include "squip/squirrel_error.hpp"

namespace squip {

Object::Object() :
  m_vm(nullptr),
  m_handle()
{}

Object::Object(HSQUIRRELVM vm, SQInteger idx) :
  m_vm(vm),
  m_handle()
{
  sq_resetobject(&m_handle);
  if (SQ_FAILED(sq_getstackobj(vm, idx, &m_handle))) {
    throw SquirrelError(m_vm, "failed to get object from stack");
  }

  sq_addref(m_vm, &m_handle);
}

Object::~Object()
{
  release();
}

Object::Object(Object const& other) :
  m_vm(other.m_vm),
  m_handle(other.m_handle)
{
  sq_addref(m_vm, &m_handle);
}

Object&
Object::operator=(Object const& other)
{
  if (&other == this) { return *this; }

  m_vm = other.m_vm;
  m_handle = other.m_handle;

  sq_addref(m_vm, &m_handle);

  return *this;
}

Object::Object(Object&& other) :
  m_vm(other.m_vm),
  m_handle(other.m_handle)
{
  other.m_vm = nullptr;
}

Object&
Object::operator=(Object&& other)
{
  if (&other == this) { return *this; }

  release();

  m_vm = other.m_vm;
  m_handle = other.m_handle;

  other.m_vm = nullptr;

  return *this;
}

void
Object::push(HSQUIRRELVM vm)
{
  sq_pushobject(vm, m_handle);
}

void
Object::release()
{
  if (m_vm != nullptr) {
    sq_release(m_vm, &m_handle);
    m_vm = nullptr;
  }
}

} // namespace squip

/* EOF */

