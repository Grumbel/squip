// squip - Utilities for Squirrel
// Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//               2023 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_SQUIP_SQUIRREL_ERROR_HPP
#define HEADER_SQUIP_SQUIRREL_ERROR_HPP

#include <stdexcept>
#include <string>
#include <string_view>

#include <squirrel.h>

namespace squip {

/** Exception class for squirrel errors, it takes a squirrelvm and uses
 * sq_geterror() to retrieve additional information about the last error that
 * occurred and creates a readable message from that.
 */
class SquirrelError final : public std::exception
{
public:
  SquirrelError(HSQUIRRELVM vm, std::string_view message) noexcept;
  SquirrelError(SquirrelError const&) = default;
  SquirrelError& operator=(SquirrelError const&) = default;
  ~SquirrelError() noexcept override;

  virtual char const* what() const noexcept override;

private:
  std::string m_message;
};

} // namespace squip

#endif

/* EOF */
