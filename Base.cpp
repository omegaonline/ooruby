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

VALUE obj_IObject;

static VALUE object_qi(VALUE self, VALUE arg) 
{
	return Qnil;
}

static void object_free(void* p) 
{
	
}

static VALUE object_new_i(int argc, VALUE *argv, VALUE klass)
{
	// Args: (const string_t& strURI, (optional) Activation::Flags_t flags, (optional) IObject* pOuter)
	if (argc < 1 || argc > 3)
		rb_raise(rb_eArgError,"Invalid number of arguments supplied");

	// Sort out URI
	string_t strURI(STR2CSTR(argv[0]),false);
	
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
	
	IObject* pObject = 0;
	guid_t iid = guid_t::Null();
	g_ptrApartment->CreateInstance(strURI,flags,pOuter,iid,pObject);
	pObject->Release();

	return Qnil;
}

static VALUE object_new(int argc, VALUE *argv, VALUE klass)
{
	try
	{
		return object_new_i(argc,argv,klass);
	}
	catch (IException* pE)
	{
		throw_exception(pE);		
	}
}

void define_basic_interfaces(VALUE mod_omega)
{
	// Define the basic Omega::IObject type
	obj_IObject = rb_define_class_under(mod_omega,"IObject",rb_cObject);
	rb_define_singleton_method(obj_IObject,"new",RUBY_METHOD_FUNC(&object_new),-1);
	rb_define_method(obj_IObject,"QueryInterface",RUBY_METHOD_FUNC(&object_qi),1);
}
