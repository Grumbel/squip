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

#ifndef UNPACK_HPP
#define UNPACK_HPP

#include <iostream>
#include <vector>

#include <squirrel.h>

#include "squip/squirrel_error.hpp"

namespace squip {

template<typename T>
T unpack(HSQUIRRELVM vm, SQInteger idx) = delete;

template<>
SQBool unpack<SQBool>(HSQUIRRELVM vm, SQInteger idx)
{
  SQBool value;
  if (SQ_FAILED(sq_getbool(vm, idx, &value))) {
    throw SquirrelError(vm, "failed to retrieve bool");
  }
  return value;
}

template<>
SQInteger unpack<SQInteger>(HSQUIRRELVM vm, SQInteger idx)
{
  SQInteger value;
  if (SQ_FAILED(sq_getinteger(vm, idx, &value))) {
    throw SquirrelError(vm, "failed to retrieve integer");
  }
  return value;
}

template<>
SQFloat unpack<SQFloat>(HSQUIRRELVM vm, SQInteger idx)
{
  SQFloat value;
  if (SQ_FAILED(sq_getfloat(vm, idx, &value))) {
    throw SquirrelError(vm, "failed to retrieve float");
  }
  return value;
}

template<>
SQChar const* unpack<SQChar const*>(HSQUIRRELVM vm, SQInteger idx)
{
  SQChar const* value;
  if (SQ_FAILED(sq_getstring(vm, idx, &value))) {
    throw SquirrelError(vm, "failed to retrieve string");
  }
  return value;
}

template<typename T>
std::vector<T> unpack_array(HSQUIRRELVM vm, SQInteger idx)
{
  std::vector<T> value;

  sq_pushnull(vm);  // iterator
  while (SQ_SUCCEEDED(sq_next(vm, idx)))
  {
    // here -1 is the value and -2 is the key
    // we ignore the key, since that is just the index in an array
    value.emplace_back(unpack<T>(vm, -1));

    sq_pop(vm, 2); //pops key and val before the nex iteration
  }
  sq_pop(vm, 1);

  return value;
}

template<>
std::vector<SQBool> unpack<std::vector<SQBool>>(HSQUIRRELVM vm, SQInteger idx)
{
  return unpack_array<SQBool>(vm, idx);
}

template<>
std::vector<SQInteger> unpack<std::vector<SQInteger>>(HSQUIRRELVM vm, SQInteger idx)
{
  return unpack_array<SQInteger>(vm, idx);
}

template<>
std::vector<SQFloat> unpack<std::vector<SQFloat>>(HSQUIRRELVM vm, SQInteger idx)
{
  return unpack_array<SQFloat>(vm, idx);
}

template<>
std::vector<SQChar const*> unpack<std::vector<SQChar const*>>(HSQUIRRELVM vm, SQInteger idx)
{
  return unpack_array<SQChar const*>(vm, idx);
}

template<typename T, typename... Args>
std::tuple<T, Args...> unpack_args(HSQUIRRELVM vm, SQInteger idx = 2)
{
  if constexpr (sizeof...(Args) == 0) {
    return std::tuple<T>{unpack<T>(vm, idx)};
  } else {
    return std::tuple_cat(std::tuple<T>{unpack<T>(vm, idx)},
                          unpack_args<Args...>(vm, idx + 1));
  }
}

} // namespace squip

#endif
