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

#include "./OORuby.h"

#ifdef OMEGA_HAVE_VLD
#include <vld.h>
#endif

// This allows us to register a clean-up function for the extension
static void register_terminator(void (*term_fn)())
{
	static int terminator = 0;
	static VALUE termHolder = Data_Wrap_Struct(rb_cObject,0,term_fn,&terminator);

	rb_global_variable(&termHolder);
}

void Term_OORuby()
{
	// Done with Omega
	Omega::Uninitialize();
}

extern "C" void OMEGA_EXPORT Init_OORuby()
{
	// Call Omega::Initialize safely
	Omega::IException* pE = Omega::Initialize();
	if (pE)
	{
		std::string strSrc = pE->Source().ToUTF8();
		std::string strDesc = pE->Description().ToUTF8();
		pE->Release();

		rb_fatal("Omega exception returned by Omega::Initialize. %s.  Source: %s",strDesc.c_str(),strSrc.c_str());
	}

	// Register a terminator
	register_terminator(Term_OORuby);

	OTL::ObjectPtr<Omega::Apartment::IApartment> ptrApartment = Omega::Apartment::IApartment::Create(Omega::guid_t::Null());
}
