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

ObjectPtr<Compartment::ICompartment> g_ptrCompartment;

void throw_exception(IException* pE)
{
	std::string strDesc = pE->GetDescription().ToNative();
	pE->Release();

	rb_fatal("Omega exception thrown: %s",strDesc.c_str());
}

static VALUE omega_createinstance_i(int argc, VALUE *argv)
{
	// Args: (const string_t& strURI, (optional) Activation::Flags_t flags, (optional) IObject* pOuter)
	if (argc < 1 || argc > 3)
		rb_raise(rb_eArgError,"Invalid number of arguments supplied");

	// Sort out URI
	string_t strURI(rb_string_value_cstr(argv[0]),false);

	// Sort out flags
	Activation::Flags_t flags = Activation::Any;
	if (argc >= 2)
		flags = static_cast<Activation::Flags_t>(NUM2UINT(argv[1]));

	// Sort out pOuter
	IObject* pOuter = 0;
	if (argc >= 3)
	{
		// t = TYPE(argv[2]);
	}

	// try to create the object asking for TypeInfo::IProvideObjectInfo
	IObject* pObject = 0;
	g_ptrCompartment->CreateInstance(strURI,flags,pOuter,OMEGA_GUIDOF(TypeInfo::IProvideObjectInfo),pObject);

	ObjectPtr<TypeInfo::IProvideObjectInfo> ptrPOI;
	ptrPOI.Attach(static_cast<TypeInfo::IProvideObjectInfo*>(pObject));
	pObject = 0;

	// Try to get the first interface
	IEnumGuid* pEG = ptrPOI->EnumInterfaces();
	ObjectPtr<IEnumGuid> ptrEG;
	ptrEG.Attach(static_cast<IEnumGuid*>(pEG));

	uint32_t count = 1;
	guid_t iid;
	ptrEG->Next(count,&iid);
	if (count==0)
		OMEGA_THROW("Object has no scriptable interfaces!");

	return create_instance(iid,ptrPOI);
}

static VALUE omega_createinstance(int argc, VALUE *argv, VALUE /*klass*/)
{
	try
	{
		return omega_createinstance_i(argc,argv);
	}
	catch (IException* pE)
	{
		throw_exception(pE);
	}
}

void module_init()
{
	// Create an apartment for all our stuff...
	g_ptrCompartment.Attach(Compartment::ICompartment::Create());

	// Define our module
	VALUE mod_omega = rb_define_module("Omega");

	// Define our guid type
	define_guid_t(mod_omega);

	// Define the global functions
	rb_define_module_function(mod_omega,"CreateInstance",RUBY_METHOD_FUNC(&omega_createinstance),-1);

	// Define the basic Omega interface types
	define_basic_interfaces(mod_omega);
}

void module_term()
{
	g_ptrCompartment.Release();
}

