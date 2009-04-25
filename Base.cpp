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

#include "./Base.h"
#include "./Guid.h"

#include <OOCore/Preprocessor/Sequence.h>

class OmegaObject
{
public:
	static void DefineBases(VALUE mod_omega);
	static VALUE Create(const guid_t& iid, IObject* pObject);

	VALUE MethodCall(uint32_t method_idx, int argc, VALUE *argv);
		
private:
	OmegaObject(const guid_t& iid, System::IProxy* pProxy, System::IMarshaller* pMarshaller, TypeInfo::ITypeInfo* pTypeInfo) :
		m_iid(iid), m_ptrProxy(pProxy), m_ptrMarshaller(pMarshaller), m_ptrTypeInfo(pTypeInfo)
	{}

	static void Free(void* p);
	static VALUE QueryInterface(VALUE self, VALUE arg);
	static VALUE GetInterfaceClass(const guid_t& iid, TypeInfo::ITypeInfo* pTI, uint32_t& method_offset);

	VALUE ReadVal(Remoting::IMessage* pParamsIn, const wchar_t* strName, TypeInfo::Types_t type);
	void WriteVal(Remoting::IMessage* pParamsOut, VALUE param, const wchar_t* strName, TypeInfo::Types_t type);

	guid_t                         m_iid;
	ObjectPtr<System::IProxy>      m_ptrProxy;
	ObjectPtr<System::IMarshaller> m_ptrMarshaller;
	ObjectPtr<TypeInfo::ITypeInfo> m_ptrTypeInfo;

	static VALUE                   s_classIObject;
	static std::map<guid_t,VALUE>  s_mapClasses;

	template <uint32_t I>
	struct Thunk
	{
		static VALUE Call(int argc, VALUE *argv, VALUE self)
		{
			Check_Type(self, T_DATA);
			OmegaObject* pThis = (OmegaObject*)DATA_PTR(self);

			try
			{
				return pThis->MethodCall(I,argc,argv);
			}
			catch (IException* pE)
			{
				throw_exception(pE);		
			}
		}
	};
	typedef VALUE (*ThunkPtr)(int argc, VALUE *argv, VALUE self);
	static ThunkPtr GetThunk(uint32_t method_idx);
};

// The static data
VALUE OmegaObject::s_classIObject;
std::map<guid_t,VALUE> OmegaObject::s_mapClasses;

// The thunk table
#define OUTPUT_THUNK(i) &Thunk<i>::Call,

OmegaObject::ThunkPtr OmegaObject::GetThunk(uint32_t method_idx)
{
	static ThunkPtr thunks[] = 
	{
		OMEGA_REPEAT(OMEGA_MAX_DEFINES,OUTPUT_THUNK)
		0
	};

	if (method_idx >= OMEGA_MAX_DEFINES)
		OMEGA_THROW(L"Too many methods in interface!");

	return thunks[method_idx];
}

VALUE OmegaObject::Create(const guid_t& iid, IObject* pObject)
{
	// Confirm we can QI for all the interfaces we need...
	ObjectPtr<System::IProxy> ptrProxy(pObject);

	ObjectPtr<System::IMarshaller> ptrMarshaller;
	ptrMarshaller.Attach(ptrProxy->GetMarshaller());

	ObjectPtr<TypeInfo::ITypeInfo> ptrTI;
	ptrTI.Attach(ptrMarshaller->GetTypeInfo(iid));

	// Get the class
	uint32_t method_offset = 0;
	VALUE klass = GetInterfaceClass(iid,ptrTI,method_offset);

	// Now create an instance to hold it...
	OmegaObject* pObj = 0;
	OMEGA_NEW(pObj,OmegaObject(iid,ptrProxy,ptrMarshaller,ptrTI));
	
	return Data_Wrap_Struct(klass,NULL,&Free,pObj);
}

VALUE OmegaObject::QueryInterface(VALUE self, VALUE arg) 
{
	Check_Type(self, T_DATA);
	OmegaObject* pThis = (OmegaObject*)DATA_PTR(self);

	return Qnil;
}

void OmegaObject::Free(void* p) 
{
	delete static_cast<OmegaObject*>(p);
}

void OmegaObject::WriteVal(Remoting::IMessage* pParamsOut, VALUE param, const wchar_t* strName, TypeInfo::Types_t type)
{
	switch (type & TypeInfo::typeMask)
	{
	case TypeInfo::typeBool:
		{
			bool_t v;
			if (CLASS_OF(param) == T_TRUE || param == Qtrue)
				v = true;
			else if (CLASS_OF(param) == T_FALSE || param == Qfalse)
				v = false;
			else
				v = (NUM2INT(param)==0 ? false : true);

			return pParamsOut->WriteBooleans(strName,1,&v);
		}

	case TypeInfo::typeByte:
		{
			byte_t v = static_cast<byte_t>(NUM2UINT(param));
			return pParamsOut->WriteBytes(strName,1,&v);
		}

	case TypeInfo::typeInt16:
		{
			int16_t v = static_cast<int16_t>(NUM2UINT(param));
			return pParamsOut->WriteInt16s(strName,1,&v);
		}

	case TypeInfo::typeUInt16:
		{
			uint16_t v = static_cast<uint16_t>(NUM2UINT(param));
			return pParamsOut->WriteUInt16s(strName,1,&v);
		}

	case TypeInfo::typeInt32:
		{
			int32_t v = static_cast<int32_t>(NUM2ULONG(param));
			return pParamsOut->WriteInt32s(strName,1,&v);
		}

	case TypeInfo::typeUInt32:
		{
			uint32_t v = static_cast<uint32_t>(NUM2ULONG(param));
			return pParamsOut->WriteUInt32s(strName,1,&v);
		}

	case TypeInfo::typeInt64:
		{
			int64_t v = static_cast<int64_t>(NUM2LL(param));
			return pParamsOut->WriteInt64s(strName,1,&v);
		}

	case TypeInfo::typeUInt64:
		{
			uint64_t v = static_cast<uint64_t>(NUM2ULL(param));
			return pParamsOut->WriteUInt64s(strName,1,&v);
		}

	case TypeInfo::typeFloat4:
		{
			float4_t v = static_cast<float4_t>(NUM2DBL(param));
			return pParamsOut->WriteFloat4s(strName,1,&v);
		}

	case TypeInfo::typeFloat8:
		{
			float8_t v = static_cast<float8_t>(NUM2DBL(param));
			return pParamsOut->WriteFloat8s(strName,1,&v);
		}

	case TypeInfo::typeString:
		{
			string_t v(STR2CSTR(param),false);
			return pParamsOut->WriteStrings(strName,1,&v);
		}

	case TypeInfo::typeGuid:
		{
			guid_t v = val_to_guid(param);
			return pParamsOut->WriteGuids(strName,1,&v);
		}

	case TypeInfo::typeObject:
		{
			if (rb_obj_is_kind_of(param,s_classIObject) != Qtrue)
				rb_raise(rb_eTypeError,"%s is not derived from Omega::IObject",rb_obj_classname(param));
				
			void* TODO;
			break;
		}

	case TypeInfo::typeVoid:
		OMEGA_THROW(L"Attempting to marshall void type!");

	case TypeInfo::typeUnknown:
	default:
		OMEGA_THROW(L"Attempting to marshall unknown type!");
	}
}

VALUE OmegaObject::ReadVal(Remoting::IMessage* pParamsIn, const wchar_t* strName, TypeInfo::Types_t type)
{
	switch (type & TypeInfo::typeMask)
	{
	case TypeInfo::typeBool:
		{
			bool_t v;
			pParamsIn->ReadBooleans(strName,1,&v);
			return (v == true ? Qtrue : Qfalse);
		}

	case TypeInfo::typeByte:
		{
			byte_t v;
			pParamsIn->ReadBytes(strName,1,&v);
			return INT2NUM(v);
		}

	case TypeInfo::typeInt16:
		{
			int16_t v;
			pParamsIn->ReadInt16s(strName,1,&v);
			return INT2NUM(v);
		}

	case TypeInfo::typeUInt16:
		{
			uint16_t v;
			pParamsIn->ReadUInt16s(strName,1,&v);
			return UINT2NUM(v);
		}

	case TypeInfo::typeInt32:
		{
			int32_t v;
			pParamsIn->ReadInt32s(strName,1,&v);
			return LONG2NUM(v);
		}

	case TypeInfo::typeUInt32:
		{
			uint32_t v;
			pParamsIn->ReadUInt32s(strName,1,&v);
			return ULONG2NUM(v);
		}

	case TypeInfo::typeInt64:
		{
			int64_t v;
			pParamsIn->ReadInt64s(strName,1,&v);
			return LL2NUM(v);
		}

	case TypeInfo::typeUInt64:
		{
			uint64_t v;
			pParamsIn->ReadUInt64s(strName,1,&v);
			return ULL2NUM(v);
		}

	case TypeInfo::typeFloat4:
		{
			float4_t v;
			pParamsIn->ReadFloat4s(strName,1,&v);
			return rb_float_new(v);
		}

	case TypeInfo::typeFloat8:
		{
			float8_t v;
			pParamsIn->ReadFloat8s(strName,1,&v);
			return rb_float_new(v);
		}

	case TypeInfo::typeString:
		{
			string_t v;
			pParamsIn->ReadStrings(strName,1,&v);
			return rb_str_new2(v.ToUTF8().c_str());
		}

	case TypeInfo::typeGuid:
		{
			guid_t v;
			pParamsIn->ReadGuids(strName,1,&v);
			return guid_to_val(v);
		}

	case TypeInfo::typeObject:
		{
			void* TODO;
			OMEGA_THROW(L"GAH!");
		}

	case TypeInfo::typeVoid:
		OMEGA_THROW(L"Attempting to marshall void type!");

	case TypeInfo::typeUnknown:
	default:
		OMEGA_THROW(L"Attempting to marshall unknown type!");
	}
}

VALUE OmegaObject::MethodCall(uint32_t method_idx, int argc, VALUE *argv) 
{
	string_t strName;
	TypeInfo::MethodAttributes_t attribs;
	uint32_t timeout;
	byte_t param_count;
	TypeInfo::Types_t return_type;

	m_ptrTypeInfo->GetMethodInfo(method_idx,strName,attribs,timeout,param_count,return_type);

	ObjectPtr<Remoting::IMessage> ptrParamsOut;
	ptrParamsOut.Attach(m_ptrMarshaller->CreateMessage());

	ptrParamsOut->WriteStructStart(L"ipc_request",L"$ipc_request_type");

	m_ptrProxy->WriteKey(ptrParamsOut);
	ptrParamsOut->WriteGuids(L"$iid",1,&m_iid);
	ptrParamsOut->WriteUInt32s(L"$method_id",1,&method_idx);
	
	int ret_count = (return_type == TypeInfo::typeVoid ? 0 : 1);
	int cur_arg = 0;
	for (byte_t param_idx=0;param_idx<param_count;++param_idx)
	{
		string_t strName;
		TypeInfo::Types_t type;
		TypeInfo::ParamAttributes_t attribs;
		m_ptrTypeInfo->GetParamInfo(method_idx,param_idx,strName,type,attribs);

		if (attribs & TypeInfo::attrIn)
		{
			if (cur_arg == argc)
				rb_raise(rb_eArgError,"Invalid number of arguments supplied.");

			if (type & TypeInfo::typeArray)
			{
				// Array handling...
				void* TODO;
				OMEGA_THROW(L"GAH!");
			}

			WriteVal(ptrParamsOut,argv[cur_arg++],strName.c_str(),type);
		}
		
		if (attribs & TypeInfo::attrOut)
			++ret_count;
	}

	ptrParamsOut->WriteStructEnd(L"ipc_request");

	Remoting::IMessage* pParamsIn = 0;
	IException* pE = m_ptrMarshaller->SendAndReceive(attribs,ptrParamsOut,pParamsIn,timeout);
	if (pE)
		throw pE;

	ObjectPtr<Remoting::IMessage> ptrParamsIn;
	ptrParamsIn.Attach(pParamsIn);

	VALUE retval = Qnil;
	if (ret_count == 0)
		return retval;
	else if (ret_count > 1)
		retval = rb_ary_new2(ret_count);

	if (!(attribs & TypeInfo::Asynchronous))
	{
		if (return_type != TypeInfo::typeVoid)
		{
			VALUE v = ReadVal(pParamsIn,L"$retval",return_type);
			if (ret_count == 1)
				retval = v;
			else
				rb_ary_push(retval,v);
		}
				
		for (byte_t param_idx=0;param_idx<param_count;++param_idx)
		{
			string_t strName;
			TypeInfo::Types_t type;
			TypeInfo::ParamAttributes_t attribs;
			m_ptrTypeInfo->GetParamInfo(method_idx,param_idx,strName,type,attribs);

			if (attribs & TypeInfo::attrOut)
			{
				VALUE v = Qnil;
				if (type & TypeInfo::typeArray)
				{
					// Array handling...
					void* TODO;
					OMEGA_THROW(L"GAH!");
				}
				else
					v = ReadVal(pParamsIn,strName.c_str(),type);

				if (ret_count == 1)
					retval = v;
				else
					rb_ary_push(retval,v);
			}
		}
	}

	return retval;
}

VALUE OmegaObject::GetInterfaceClass(const guid_t& iid, TypeInfo::ITypeInfo* pTI, uint32_t& method_offset)
{
	// See if we have had is before
	try
	{
		std::map<guid_t,VALUE>::iterator i = s_mapClasses.find(iid);
		if (i != s_mapClasses.end())
			return i->second;
	}
	catch (std::exception& e)
	{
		rb_fatal(e.what());
	}

	// See if we have a base class...
	VALUE base_type = rb_cObject;
	{
		ObjectPtr<TypeInfo::ITypeInfo> ptrTI;
		ptrTI.Attach(pTI->GetBaseType());
		if (ptrTI)
			base_type = GetInterfaceClass(pTI->GetIID(),ptrTI,method_offset);
		else
		{
			// It's IObject...
			method_offset = 3;
			return s_classIObject;
		}
	}

	// Create all the modules (namespaces)
	string_t strName = pTI->GetName();
	size_t start = 0;
	VALUE module = Qnil;
	for (;;)
	{
		size_t end = strName.Find(L"::",start);
		if (end == string_t::npos)
			break;

		string_t strNSpace = strName.Mid(start,end - start);
		start = end + 2;

		if (module == Qnil)
			module = rb_define_module(strNSpace.ToUTF8().c_str());
		else
			module = rb_define_module_under(module,strNSpace.ToUTF8().c_str());
	}

	// Now create the class under module
	VALUE klass = rb_define_class_under(module,strName.Mid(start).ToUTF8().c_str(),base_type);
	
	// Now add all the methods...
	uint32_t methods = pTI->GetMethodCount();
	for (uint32_t method_idx = method_offset;method_idx<methods;++method_idx)
	{
		TypeInfo::MethodAttributes_t attribs;
		uint32_t timeout;
		byte_t param_count;
		TypeInfo::Types_t return_type;

		pTI->GetMethodInfo(method_idx,strName,attribs,timeout,param_count,return_type);

		// Define a method
		rb_define_method(klass,strName.ToUTF8().c_str(),RUBY_METHOD_FUNC(GetThunk(method_idx)),-1);
	}

	// Don't let derived classes override these methods...
	method_offset = methods;

	// Insert it into the map
	try
	{
		s_mapClasses.insert(std::map<guid_t,VALUE>::value_type(iid,klass));
	}
	catch (std::exception& e)
	{
		rb_fatal(e.what());
	}

	return klass;
}

void OmegaObject::DefineBases(VALUE mod_omega)
{
	// Define the basic Omega::IObject type
	s_classIObject = rb_define_class_under(mod_omega,"IObject",rb_cObject);
	rb_define_method(s_classIObject,"QueryInterface",RUBY_METHOD_FUNC(&OmegaObject::QueryInterface),1);

	try
	{
		s_mapClasses.insert(std::map<guid_t,VALUE>::value_type(OMEGA_GUIDOF(IObject),s_classIObject));
	}
	catch (std::exception& e)
	{
		rb_fatal(e.what());
	}
}

VALUE create_instance(const guid_t& iid, IObject* pObject)
{
	return OmegaObject::Create(iid,pObject);
}

void define_basic_interfaces(VALUE mod_omega)
{
	OmegaObject::DefineBases(mod_omega);
}
