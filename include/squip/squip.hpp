#ifndef SQUIP_HPP
#define SQUIP_HPP

#include <string>

#include "exposed_object.hpp"
#include "log.hpp"
#include "script_interface.hpp"
#include "serialize.hpp"
#include "squip.hpp"
#include "squirrel_environment.hpp"
#include "squirrel_error.hpp"
#include "squirrel_scheduler.hpp"
#include "squirrel_thread_queue.hpp"
#include "squirrel_util.hpp"
#include "squirrel_virtual_machine.hpp"
#include "squirrel_vm.hpp"
#include "time.hpp"

namespace squip {

std::string hello_world();

} // namespace squip

#endif
