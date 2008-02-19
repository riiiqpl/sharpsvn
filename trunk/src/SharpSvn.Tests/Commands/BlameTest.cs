// $Id$
using System;
using System.IO;
using System.Collections;
using NUnit.Framework;
using System.Text.RegularExpressions;
using NUnit.Framework.SyntaxHelpers;

namespace SharpSvn.Tests.Commands
{
    /// <summary>
    /// Tests for the NSvn.Core.Client.Blame method.
    /// </summary>
    [TestFixture]
    public class BlameTest : TestBase
    {
        [SetUp]
        public override void SetUp()
        {
            base.SetUp ();
            this.ExtractRepos();
            this.ExtractWorkingCopy();
            this.blames = new ArrayList();
        }

        [Test]
        public void TestSimple()
        {
            string path = Path.Combine( this.WcPath, "Form.cs" );
            string blame = this.RunCommand( "svn", "blame -v " + path );
            Blame[] cmdline = this.ParseCommandLineBlame( blame );

			SvnBlameArgs a = new SvnBlameArgs();
            Assert.That(this.Client.Blame( path, a, new EventHandler<SvnBlameEventArgs>( this.Receiver ) ));
            
            Assert.AreEqual( cmdline.Length, this.blames.Count );
            for( int i = 0; i < cmdline.Length; i++ )
            {
                Blame.CheckEqual( cmdline[i], (Blame)this.blames[i] );
            }
        }

        [Test]
        public void TestWithEmptyEntries()
        {
            string path = Path.Combine( this.WcPath, "Form.cs" );

			SvnBlameArgs a = new SvnBlameArgs();
			a.Start = SvnRevision.Head;
			a.End = SvnRevision.Head;
            // this won't give any results - verify that there are no exceptions
            Assert.That(this.Client.Blame( path, a, new EventHandler<SvnBlameEventArgs>( this.Receiver ) ));

            Blame[] b = (Blame[])this.blames.ToArray( typeof(Blame) );

            Assert.AreEqual( -1, b[0].Revision );
            Assert.AreEqual( null, b[0].Author );
            Assert.AreEqual( DateTime.MinValue, b[0].Date );

        }

        private void Receiver(object sender, SvnBlameEventArgs e)
        {
            this.blames.Add( new Blame( e.LineNumber, e.Revision, e.Author, e.Time, e.Line ) );
        }

        private Blame[] ParseCommandLineBlame( string blame )
        {
            ArrayList blames = new ArrayList();
            long lineNumber = 0;
            foreach( Match m in BlameRegex.Matches( blame ) )
            {
                int revision = int.Parse( m.Groups["rev"].Value );
                string author = m.Groups["author"].Value;
                DateTime date = DateTime.ParseExact( m.Groups["date"].Value,
                    @"yyyy-MM-dd\ HH:mm:ss\ zzzz",
                    System.Globalization.CultureInfo.CurrentCulture ).ToUniversalTime();
                string line = m.Groups["line"].Value.TrimEnd('\r');
                blames.Add( new Blame( lineNumber++, revision, author, date, line ));
            }

            return (Blame[])blames.ToArray( typeof(Blame) );
        }

        private class Blame
        {
            public long LineNumber;
			public long Revision;
            public string Author;
            public DateTime Date;
            public string Line;

            public Blame( long lineNumber, long revision, string author, 
                DateTime date, string line )
            {
                this.LineNumber = lineNumber;
                this.Revision = revision;
                this.Author = author;
                this.Date = date;
                this.Line = line;
            }

            public static void CheckEqual( Blame a, Blame b )
            {
                Assert.That(a.LineNumber, Is.EqualTo(b.LineNumber));
                Assert.That(a.Revision, Is.EqualTo(b.Revision));
                Assert.That(a.Author, Is.EqualTo(b.Author));
				Assert.That(a.Date, Is.EqualTo(b.Date));
                Assert.That(a.Line, Is.EqualTo(b.Line));
            }

            private static readonly TimeSpan Second = new TimeSpan( 0, 0, 0, 1 );
        }

        private readonly Regex BlameRegex = new Regex( 
            @"\s+(?<rev>\d+)\s+(?<author>\w+)\s+(?<date>\d\d\d\d-\d\d-\d\d \d\d:\d\d:\d\d [-+]\d\d\d\d) (\(\w{1,4}, \d\d \w{1,4} \d{4}\) )?(?<line>.*)" );

        private ArrayList blames;
    }
}
