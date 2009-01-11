// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnLookRevisionPropertyArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, [Out] String^% value)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetRevisionProperty(lookOrigin, propertyName, gcnew SvnLookRevisionPropertyArgs(), value);
}

bool SvnLookClient::GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, [Out] SvnPropertyValue^% value)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetRevisionProperty(lookOrigin, propertyName, gcnew SvnLookRevisionPropertyArgs(), value);
}

bool SvnLookClient::GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, SvnLookRevisionPropertyArgs^ args, [Out] String^% result)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnPropertyValue^ value = nullptr;
	result = nullptr;

	try
	{
		return GetRevisionProperty(lookOrigin, propertyName, args, value);
	}
	finally
	{
		if (value)
			result = value->StringValue;
	}
}

bool SvnLookClient::GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, SvnLookRevisionPropertyArgs^ args, [Out] SvnPropertyValue^% value)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	value = nullptr;

	svn_error_t* r;
	svn_repos_t* repos;
	if (r = svn_repos_open(&repos, pool.AllocCanonical(lookOrigin->RepositoryPath), pool.Handle))
		return args->HandleResult(this, r);

	svn_fs_t* fs = svn_repos_fs(repos);

	svn_string_t* val;

	const char* pName = pool.AllocString(propertyName);
	
	if (lookOrigin->HasTransaction)
	{
		svn_fs_txn_t* txn = NULL;
		if (r = svn_fs_open_txn(&txn, fs, pool.AllocString(lookOrigin->Transaction), pool.Handle))
			return args->HandleResult(this, r);

		if (r = svn_fs_txn_prop(&val, txn, pName, pool.Handle))
			return args->HandleResult(this, r);
	}
	else
	{
		svn_revnum_t rev;

		if (!lookOrigin->HasRevision)
		{
			if (r = svn_fs_youngest_rev(&rev, fs, pool.Handle))
				return args->HandleResult(this, r);
		}
		else
		{
			rev = (svn_revnum_t)lookOrigin->Revision;
		}

		if (r = svn_fs_revision_prop(&val, fs, rev, pName, pool.Handle))
			return args->HandleResult(this, r);
	}

	if(val)
		value = SvnPropertyValue::Create(pName, val, nullptr, propertyName);

	return args->HandleResult(this, r);
}

