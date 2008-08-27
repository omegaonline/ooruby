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

VALUE obj_guid_t;

static void guid_t_free(void* p) 
{
	delete static_cast<guid_t*>(p);
}

static VALUE guid_t_new(int argc, VALUE *argv, VALUE klass)
{
	string_t strVal;
	if (argc == 1)
		strVal = string_t(STR2CSTR(argv[0]),false);
	else if (argc > 1)
		rb_raise(rb_eArgError,"Invalid number of arguments supplied");

	guid_t* pg = new guid_t();
	if (!pg)
		rb_raise(rb_eNoMemError,"Out of memory"); 

	if (argc == 1)
	{
		try
		{
			*pg = guid_t::FromString(strVal);
		}
		catch (IException* pE)
		{
			throw_exception(pE);
		}
	}
		
	return Data_Wrap_Struct(klass,0,guid_t_free,pg);
}

void define_guid_t(VALUE mod_omega)
{
	obj_guid_t = rb_define_class_under(mod_omega,"Guid",rb_cObject);
	rb_define_singleton_method(obj_guid_t,"new",RUBY_METHOD_FUNC(&guid_t_new),-1);
}

guid_t get_guid(VALUE val)
{
	if (rb_obj_is_instance_of(val,obj_guid_t) != Qtrue)
		rb_raise(rb_eTypeError,"%s is not a Omega::Guid",rb_obj_classname(val));

	return *(guid_t*)DATA_PTR(val);	
}
