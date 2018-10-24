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

#include <algorithm>

#include "squirrel/squirrel_virtual_machine.hpp"
#include "squirrel/squirrel_util.hpp"
#include "squirrel/time_scheduler.hpp"
#include "util/log.hpp"

TimeScheduler::TimeScheduler() :
  m_vm(SquirrelVirtualMachine::current()->get_vm()),
  schedule()
{
}

void
TimeScheduler::update(float time)
{
  while(!schedule.empty() && schedule.front().wakeup_time < time) {
    HSQOBJECT thread_ref = schedule.front().thread_ref;

    sq_pushobject(m_vm, thread_ref);
    sq_getweakrefval(m_vm, -1);

    HSQUIRRELVM scheduled_vm;
    if(sq_gettype(m_vm, -1) == OT_THREAD &&
       SQ_SUCCEEDED(sq_getthread(m_vm, -1, &scheduled_vm))) {
      if(SQ_FAILED(sq_wakeupvm(scheduled_vm, SQFalse, SQFalse, SQTrue, SQFalse))) {
        std::ostringstream msg;
        msg << "Error waking VM: ";
        sq_getlasterror(scheduled_vm);
        if(sq_gettype(scheduled_vm, -1) != OT_STRING) {
          msg << "(no info)";
        } else {
          const char* lasterr;
          sq_getstring(scheduled_vm, -1, &lasterr);
          msg << lasterr;
        }
        log_warning << msg.str() << std::endl;
        sq_pop(scheduled_vm, 1);
      }
    }

    sq_release(m_vm, &thread_ref);
    sq_pop(m_vm, 2);

    std::pop_heap(schedule.begin(), schedule.end());
    schedule.pop_back();
  }
}

void
TimeScheduler::schedule_thread(HSQUIRRELVM scheduled_vm, float time)
{
  // create a weakref to the VM
  SQObject vm_obj = vm_to_object(scheduled_vm);
  sq_pushobject(m_vm, vm_obj);
  sq_weakref(m_vm, -1);

  ScheduleEntry entry;
  if(SQ_FAILED(sq_getstackobj(m_vm, -1, & entry.thread_ref))) {
    sq_pop(m_vm, 2);
    throw SquirrelError(m_vm, "Couldn't get thread weakref from vm");
  }
  entry.wakeup_time = time;

  sq_addref(m_vm, & entry.thread_ref);
  sq_pop(m_vm, 2);

  schedule.push_back(entry);
  std::push_heap(schedule.begin(), schedule.end());
}

/* EOF */
