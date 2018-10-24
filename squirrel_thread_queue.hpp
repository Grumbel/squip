//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SUPERTUX_SQUIRREL_SQUIRREL_THREAD_QUEUE_HPP
#define HEADER_SUPERTUX_SQUIRREL_SQUIRREL_THREAD_QUEUE_HPP

#include <squirrel.h>
#include <vector>

/**
 * Keeps a list of SquirrelThreads that wait for a wakeup event
 */
class SquirrelThreadQueue final
{
public:
  SquirrelThreadQueue(HSQUIRRELVM vm);

  /** adds a thread (actually a weakref to the thread) */
  void add(HSQUIRRELVM vm);

  /** wakes up threads in the list */
  void wakeup();

private:
  HSQUIRRELVM m_vm;
  std::vector<HSQOBJECT> m_threads;

private:
  SquirrelThreadQueue(const SquirrelThreadQueue&) = delete;
  SquirrelThreadQueue& operator=(const SquirrelThreadQueue&) = delete;
};

#endif

/* EOF */
