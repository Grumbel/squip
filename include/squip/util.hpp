// squip - Utilities for Squirrel
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
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

#ifndef HEADER_SQUIP_UTIL_HPP
#define HEADER_SQUIP_UTIL_HPP

#include <cassert>
#include <functional>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "squip/squirrel_error.hpp"

namespace squip {

/** Pick object from stack position idx and print it to a machine readable string */
void repr(HSQUIRRELVM vm, SQInteger idx, std::ostream& os);

/** Pick object from stack position idx and print it to a human readable string */
void print(HSQUIRRELVM vm, SQInteger idx, std::ostream& os);

/** Pick object from stack position idx and convert it to a machine readable string */
std::string to_repr(HSQUIRRELVM vm, SQInteger idx);

/** Pick object from stack position idx and convert it to a human readable string */
std::string to_string(HSQUIRRELVM vm, SQInteger idx);

void print_stack(HSQUIRRELVM vm, std::ostream& os);
void print_stacktrace(HSQUIRRELVM vm, std::ostream& os);

SQInteger squirrel_read_char(SQUserPointer file);

HSQUIRRELVM object_to_vm(HSQOBJECT object);

void compile_script(HSQUIRRELVM vm, std::istream& in,
                    const std::string& sourcename);
void compile_and_run(HSQUIRRELVM vm, std::istream& in,
                     const std::string& sourcename);

void push_function(HSQUIRRELVM vm, std::function<SQInteger (HSQUIRRELVM)> func);

} // namespace squip

#endif

/* EOF */
