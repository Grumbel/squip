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

#include "squip/squirrel_error.hpp"

#include <cassert>
#include <sstream>

#include <fmt/format.h>

namespace squip {

SquirrelError::SquirrelError(HSQUIRRELVM vm, std::string_view message) noexcept :
  m_message()
{
  SQInteger const oldtop = sq_gettop(vm);

  char const* lasterr = "no error info";
  sq_getlasterror(vm);

  if (sq_gettype(vm, -1) != OT_STRING) {
    if (SQ_FAILED(sq_tostring(vm, -1))) {
      assert(false && "never reached");
      goto end;
    }
  }

  sq_getstring(vm, -1, &lasterr);

end:
  m_message = fmt::format("SquirrelError: {} ({})", message, lasterr);

  sq_settop(vm, oldtop);
}

SquirrelError::~SquirrelError() throw()
{}

char const*
SquirrelError::what() const throw()
{
  return m_message.c_str();
}

} // namespace squip

/* EOF */
