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

#include "squip/fwd.hpp"
#include "squip/squirrel_error.hpp"

namespace squip {

/** Dummy value to allow sq_pushnull() via overloading */
struct Null {};

/** Pick object from stack position idx and print it to a machine readable string */
void repr(HSQUIRRELVM vm, SQInteger idx, std::ostream& os, bool pretty = false, int indent = 0);

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

void push_value(HSQUIRRELVM vm, SQBool value);
void push_value(HSQUIRRELVM vm, SQInteger value);
void push_value(HSQUIRRELVM vm, SQFloat value);
void push_value(HSQUIRRELVM vm, std::string_view value);
void push_value(HSQUIRRELVM vm, SQUserPointer userptr);
void push_value(HSQUIRRELVM vm, Object const& obj);
void push_value(HSQUIRRELVM vm, HSQOBJECT const& obj);
void push_value(HSQUIRRELVM vm, Null const& null);

void push_value(HSQUIRRELVM vm, char const* value);
void push_value(HSQUIRRELVM vm, bool value);
void push_value(HSQUIRRELVM vm, int value);

void write_closure(HSQUIRRELVM vm, std::ostream& out);
void read_closure(HSQUIRRELVM vm, std::istream& in);

/** Convert the given index into a positive index (e.g. -1 -> sq_gettop()) */
SQInteger absolute_index(HSQUIRRELVM vm, SQInteger idx);

TableContext new_table(HSQUIRRELVM vm);
ArrayContext new_array(HSQUIRRELVM vm, SQInteger size);

} // namespace squip

#endif

/* EOF */
