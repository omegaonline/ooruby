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

using namespace Omega;
using namespace OTL;

#include "./Guid.h"
#include "./Base.h"

ObjectPtr<Apartment::IApartment> g_ptrApartment;

void throw_exception(IException* pE)
{
	std::string strSrc = pE->GetSource().ToUTF8();
	std::string strDesc = pE->GetDescription().ToUTF8();
	pE->Release();

	rb_fatal("Omega exception thrown: %s.  Source: %s",strDesc.c_str(),strSrc.c_str());
}

void module_init()
{
	// Create an apartment for all our stuff...
	g_ptrApartment.Attach(Apartment::IApartment::Create());

	// Define our module
	VALUE mod_omega = rb_define_module("Omega");
	
	// Define our guid type
	define_guid_t(mod_omega);

	// Define the basic Omega interface types
	define_basic_interfaces(mod_omega);
}

void module_term()
{
	g_ptrApartment.Release();
}
