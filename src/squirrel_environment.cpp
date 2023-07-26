//  SuperTux
//  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
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

#include "squip/squirrel_environment.hpp"

#include <algorithm>

#include "squip/script_interface.hpp"
#include "squip/squirrel_error.hpp"
#include "squip/squirrel_scheduler.hpp"
#include "squip/squirrel_util.hpp"
#include "squip/squirrel_virtual_machine.hpp"
#include "squip/log.hpp"
#include "squip/time.hpp"

namespace squip {

SquirrelEnvironment::SquirrelEnvironment(SquirrelVM& vm, const std::string& name) :
  m_vm(vm),
  m_table(),
  m_name(name),
  m_scripts(),
  m_scheduler(std::make_unique<SquirrelScheduler>(m_vm))
{
  // garbage collector has to be invoked manually
  sq_collectgarbage(m_vm.get_vm());

  sq_newtable(m_vm.get_vm());
  sq_pushroottable(m_vm.get_vm());
  if (SQ_FAILED(sq_setdelegate(m_vm.get_vm(), -2)))
    throw SquirrelError(m_vm.get_vm(), "failed to set table delegate");

  sq_resetobject(&m_table);
  if (SQ_FAILED(sq_getstackobj(m_vm.get_vm(), -1, &m_table))) {
    throw SquirrelError(m_vm.get_vm(), "failed to get table");
  }

  sq_addref(m_vm.get_vm(), &m_table);
  sq_pop(m_vm.get_vm(), 1);
}

SquirrelEnvironment::~SquirrelEnvironment()
{
  m_scripts.clear();
  sq_release(m_vm.get_vm(), &m_table);

  sq_collectgarbage(m_vm.get_vm());
}

void
SquirrelEnvironment::expose_self()
{
#if 0
  sq_pushroottable(m_vm.get_vm());
  m_vm.store_object(m_name.c_str(), m_table);
  sq_pop(m_vm.get_vm(), 1);
#endif
}

void
SquirrelEnvironment::unexpose_self()
{
#if 0
  sq_pushroottable(m_vm.get_vm());
  m_vm.delete_table_entry(m_name.c_str());
  sq_pop(m_vm.get_vm(), 1);
#endif
}

void
SquirrelEnvironment::try_expose(GameObject& object)
{
#if 0
  auto script_object = dynamic_cast<ScriptInterface*>(&object);
  if (script_object != nullptr) {
    sq_pushobject(m_vm.get_vm(), m_table);
    script_object->expose(m_vm.get_vm(), -1);
    sq_pop(m_vm.get_vm(), 1);
  }
#endif
}

void
SquirrelEnvironment::try_unexpose(GameObject& object)
{
#if 0
  auto script_object = dynamic_cast<ScriptInterface*>(&object);
  if (script_object != nullptr) {
    SQInteger oldtop = sq_gettop(m_vm.get_vm());
    sq_pushobject(m_vm.get_vm(), m_table);
    try {
      script_object->unexpose(m_vm.get_vm(), -1);
    } catch(std::exception& e) {
      log_warning << "failed to unregister object: " << e.what() << std::endl;
    }
    sq_settop(m_vm.get_vm(), oldtop);
  }
#endif
}

void
SquirrelEnvironment::unexpose(const std::string& name)
{
  SQInteger oldtop = sq_gettop(m_vm.get_vm());
  sq_pushobject(m_vm.get_vm(), m_table);
  try {
#if 0
    unexpose_object(m_vm.get_vm(), -1, name.c_str());
#endif
  } catch(std::exception& e) {
    log_warning << "failed to unregister object: " << e.what() << std::endl;
  }
  sq_settop(m_vm.get_vm(), oldtop);
}

void
SquirrelEnvironment::run_script(const std::string& script, const std::string& sourcename)
{
  if (script.empty()) return;

  std::istringstream stream(script);
  run_script(stream, sourcename);
}

void
SquirrelEnvironment::garbage_collect()
{
  m_scripts.erase(
    std::remove_if(m_scripts.begin(), m_scripts.end(),
                   [this](Thread& thread){
                     HSQUIRRELVM vm = thread.get_vm();
                     return sq_getvmstate(vm) != SQ_VMSTATE_SUSPENDED;
                   }),
    m_scripts.end());
}

void
SquirrelEnvironment::run_script(std::istream& in, const std::string& sourcename)
{
  garbage_collect();

  try
  {
    Thread thread = m_vm.create_thread();
    HSQUIRRELVM vm = thread.get_vm();
    m_scripts.emplace_back(std::move(thread));

    sq_setforeignptr(vm, this);

    // set root table
    sq_pushobject(vm, m_table);
    sq_setroottable(vm);

    compile_and_run(vm, in, sourcename);
  }
  catch(const std::exception& e)
  {
    log_warning << "Error running script: " << e.what() << std::endl;
  }
}

void
SquirrelEnvironment::wait_for_seconds(HSQUIRRELVM vm, float seconds)
{
  m_scheduler->schedule_thread(vm, g_game_time + seconds, false);
}

void
SquirrelEnvironment::skippable_wait_for_seconds(HSQUIRRELVM vm, float seconds)
{
  m_scheduler->schedule_thread(vm, g_game_time + seconds, true);
}

void
SquirrelEnvironment::update(float dt_sec)
{
  m_scheduler->update(g_game_time);
}

} // namespace squip

/* EOF */
