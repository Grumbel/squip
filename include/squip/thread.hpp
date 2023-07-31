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

#ifndef HEADER_SQUIP_THREAD_HPP
#define HEADER_SQUIP_THREAD_HPP

#include <filesystem>

#include <squirrel.h>

#include "squip/stack_context.hpp"

namespace squip {

class SquirrelVM;

class Thread
{
public:
  Thread(SquirrelVM& sqvm);
  ~Thread();

  Thread(Thread&& other);
  Thread& operator=(Thread&& other);

  void run_script(std::filesystem::path const& path);

  void wakeup(SQBool resumedret = SQFalse, SQBool retval = SQFalse, SQBool raiseerror = SQTrue, SQBool throwerror = SQFalse);
  bool is_suspended();

  HSQUIRRELVM get_vm() { return m_vm; }

  StackContext stack() { return StackContext(m_vm); }

private:
  SquirrelVM* m_sqvm;
  HSQUIRRELVM m_vm;
  HSQOBJECT m_handle;

public:
  Thread(Thread const&) = delete;
  Thread& operator=(Thread const&) = delete;
};

} // namespace squip

#endif

/* EOF */
