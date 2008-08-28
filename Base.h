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

void define_basic_interfaces(VALUE mod_omega);

// Define our PSFactory
class PSFactory :
	public ObjectBase,
	public System::IProxyStubFactory
{
public:
	BEGIN_INTERFACE_MAP(PSFactory)
		INTERFACE_ENTRY(System::IProxyStubFactory)
	END_INTERFACE_MAP()

	// IProxyStubFactory members
public:
	void CreateProxy(const guid_t& iid, TypeInfo::ITypeInfo* pTypeInfo, System::IProxy* pOuter, System::IMarshaller* pManager, IObject*& pProxy);
	System::IStub* CreateStub(const guid_t& iid, TypeInfo::ITypeInfo* pTypeInfo, System::IStubController* pController, System::IMarshaller* pManager, IObject* pObject);
};
