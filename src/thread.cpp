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

#include "squip/thread.hpp"

#include <fstream>

#include <squirrel.h>
#include <fmt/format.h>

#include "squip/squirrel_error.hpp"
#include "squip/squirrel_util.hpp"

namespace squip {

Thread::Thread(SquirrelVM& sqvm) :
  m_sqvm(&sqvm),
  m_vm(nullptr),
  m_handle()
{
  m_vm = sq_newthread(m_sqvm->get_vm(), 64);
  if (m_vm == nullptr) {
    throw SquirrelError(m_vm, "failed to create thread");
  }

  sq_resetobject(&m_handle);
  if (SQ_FAILED(sq_getstackobj(m_sqvm->get_vm(), -1, &m_handle))) {
    throw SquirrelError(m_vm, "failed to get object from stack");
  }
  sq_addref(m_sqvm->get_vm(), &m_handle);

  sq_pop(m_sqvm->get_vm(), 1);
}

Thread::~Thread()
{
  if (m_sqvm != nullptr) {
    sq_release(m_sqvm->get_vm(), &m_handle);
  }
}

Thread::Thread(Thread&& other) :
  m_sqvm(other.m_sqvm),
  m_vm(other.m_vm),
  m_handle(other.m_handle)
{
  other.m_sqvm = nullptr;
  other.m_vm = nullptr;
  // other.m_handle = nullptr;
}

Thread&
Thread::operator=(Thread&& other)
{
  if (m_sqvm != nullptr) {
    sq_release(m_sqvm->get_vm(), &m_handle);
  }

  m_sqvm = other.m_sqvm;
  m_vm = other.m_vm;
  m_handle = other.m_handle;

  other.m_sqvm = nullptr;
  other.m_vm = nullptr;
  // other.m_handle = nullptr;

  return *this;
}

void
Thread::run_script(std::filesystem::path const& path)
{
  std::ifstream fin(path);
  if (!fin) {
    throw std::runtime_error(fmt::format("failed to open file: {}", path.string()));
  }
  squip::compile_and_run(m_vm, fin, path.string());
}

void
Thread::wakeup(SQBool resumedret, SQBool retval, SQBool raiseerror, SQBool throwerror)
{
  if (SQ_FAILED(sq_wakeupvm(m_vm, resumedret, retval, raiseerror, throwerror))) {
    throw SquirrelError(m_vm, "wakeup failed");
  }
}

bool
Thread::is_suspended()
{
  return sq_getvmstate(m_vm) == SQ_VMSTATE_SUSPENDED;
}

} // namespace squip

/* EOF */
