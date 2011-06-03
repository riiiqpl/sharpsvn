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

// $Id$
// Copyright (c) SharpSvn Project 2008, Copyright (c) Ankhsvn 2003-2007
using System;
using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using NUnit.Framework.SyntaxHelpers;
using SharpSvn;

namespace SharpSvn.Tests.Commands
{
	/// <summary>
	/// Summary description for PropGetTest.
	/// </summary>
	[TestFixture]
	public class GetPropertyTests : TestBase
	{
		[Test]
		public void TestPropGetOnFile()
		{
			string path = Path.Combine(this.WcPath, "Form.cs");
			this.RunCommand("svn", "ps foo bar " + path);

			string value;
			Assert.That(Client.GetProperty(new SvnPathTarget(path), "foo", out value));

			Assert.That(value, Is.EqualTo("bar"));

			SvnPropertyValue pval;

			Assert.That(Client.GetProperty(new SvnPathTarget(path), "foo", out pval));

			Assert.That(pval.StringValue, Is.EqualTo("bar"));
			Assert.That(pval.Key, Is.EqualTo("foo"));
			Assert.That(pval.Target.TargetName, Is.EqualTo(path));
		}

		[Test]
		public void TestNonExistentPropertyExistingFile()
		{
			string wc = GetTempDir();
			Client.CheckOut(new Uri(CollabReposUri, "trunk"), wc);

			SvnGetPropertyArgs pa = new SvnGetPropertyArgs();
			pa.ThrowOnError = false;

			string value;
			Assert.That(Client.GetProperty(Path.Combine(wc, "index.html"),
				"new-prop", out value), Is.True,
				"GetProperty succeeds on non existent property");

			Assert.That(value, Is.Null, "No value available");
		}

		[Test, ExpectedException(typeof(SvnEntryNotFoundException),
			ExpectedMessage="no-index.html", MatchType=MessageMatch.Contains)]
		public void TestNonExistentPropertyNonExistingFile()
		{
			string wc = GetTempDir();
			Client.CheckOut(new Uri(CollabReposUri, "trunk"), wc);

			string value;
			Client.GetProperty(Path.Combine(wc, "no-index.html"), "new-prop", out value);
		}

		[Test, ExpectedException(typeof(SvnInvalidNodeKindException),
			ExpectedMessage="{632382A5-F992-4ab8-8D37-47977B190819}", MatchType=MessageMatch.Contains)]
		public void TestNoWcProperty()
		{
			string value;
			Client.GetProperty("c:/{632382A5-F992-4ab8-8D37-47977B190819}/no-file.txt", "no-prop", out value);
		}
	}
}



