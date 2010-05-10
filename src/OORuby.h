///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2008 Rick Taylor
//
// This file is part of OORuby, the Omega Online Ruby extension library.
//
// OORuby is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OORuby is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OORuby.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
// Include Omega/OTL components

#include <Omega/Apartment.h>

#include <OTL/OTL.h>

// End of Omega/OTL includes
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Include Windows components
#if defined(_WIN32)

// Force Ruby to use the winsock2 definitions
#include <winsock2.h>

#endif
// End of Windows includes
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Include Ruby components

#include "ruby.h"

#if !defined(HAVE_LONG_LONG)
#error Your platform has no support for 64-bit numbers
#endif

// End of Ruby includes
/////////////////////////////////////////////////

// Global functions
NORETURN(void throw_exception(Omega::IException* pE));

// Global apartment
extern OTL::ObjectPtr<Omega::Apartment::IApartment> g_ptrApartment;
