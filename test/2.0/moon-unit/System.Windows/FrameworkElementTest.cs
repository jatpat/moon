using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.Generic;
using Mono.Moonlight.UnitTesting;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MoonTest.System.Windows
{
	[TestClass]
	public class FrameworkElementTest
	{
		[TestMethod]
		[KnownFailure]
		public void TestName ()
		{
			Assert.AreEqual ("", (new Canvas()).Name);
		}
	}
}