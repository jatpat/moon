//
// Unit tests for System.Windows.Application
//
// Contact:
//   Moonlight List (moonlight-list@lists.ximian.com)
//
// Copyright (C) 2008 Novell, Inc (http://www.novell.com)
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Resources;
using Mono.Moonlight.UnitTesting;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MoonTest.System.Windows {

	[TestClass]
	public class ApplicationTest {

		static private Uri uri = new Uri ("http://www.mono-project.com");

		void Check (Application app)
		{
			Assert.IsNotNull (app.Host, "Host");

			// not yet implemented in moonlight
//			Assert.IsNotNull (app.Resources, "Resources");

			Assert.IsNotNull (app.RootVisual, "RootVisual");
		}

		[TestMethod]
		[KnownFailure]
		public void New ()
		{
			Application app = new Application ();
			// AFAIK this is *nearly* identical to Application.Current
			Check (app);
			// funny thing is that creating a SilverlightHost defaults to true :|
			Assert.IsFalse (app.Host.IsLoaded, "Host.IsLoaded");
		}

		[TestMethod]
		[KnownFailure]
		public void Current ()
		{
			Application app = Application.Current;
			Check (app);
			Assert.IsTrue (app.Host.IsLoaded, "Host.IsLoaded");
		}

		[TestMethod]
		public void Static ()
		{
			Assert.Throws<ArgumentNullException> (() => Application.GetResourceStream (null), "GetResourceStream(null)");
			Assert.Throws<ArgumentNullException> (() => Application.GetResourceStream (null, uri), "GetResourceStream(null,uri)");

			Assert.IsNull (Application.GetResourceStream (new Uri ("/moon_unit;component/App-does-not-exists.xaml", UriKind.Relative)), "GetResourceStream(does-not-exists-uri)");

			StreamResourceInfo sri = new StreamResourceInfo (new MemoryStream (), String.Empty);

			Assert.Throws<ArgumentNullException> (() => Application.GetResourceStream (sri, null), "GetResourceStream(sri,null)");
			Assert.Throws<ArgumentNullException> (() => Application.LoadComponent (null, uri), "LoadComponent(null,uri)");
		}

		[TestMethod]
		public void LoadComponent_Application ()
		{
			// Note: Exception message can be misleading
			Assert.Throws<ArgumentNullException> (() => Application.LoadComponent (Application.Current, null), "LoadComponent(app,null)");
			Assert.Throws<ArgumentException> (() => Application.LoadComponent (Application.Current, uri), "LoadComponent(app,uri)");
			Assert.Throws<ArgumentException> (() => Application.LoadComponent (Application.Current, Application.Current.Host.Source), "LoadComponent(app,xap)");

			// try to load an unexisting uri
			Application.LoadComponent (Application.Current, new Uri ("/moon_unit;component/App-does-not-exists.xaml", UriKind.Relative));
		}

		[TestMethod]
		public void ResetRootVisual ()
		{
			UIElement root = Application.Current.RootVisual;
			Canvas new_root = new Canvas ();
			Application.Current.RootVisual = new_root;
			// we can't change the root
			Assert.IsFalse (Object.ReferenceEquals (new_root, Application.Current.RootVisual), "new root");
			Assert.IsTrue (Object.ReferenceEquals (root, Application.Current.RootVisual), "old root");
			// and we can't clear it
			Assert.Throws (delegate { Application.Current.RootVisual = null; }, typeof (InvalidOperationException), "null");
		}
	}
}
