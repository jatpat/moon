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


namespace MoonTest.System.Windows.Controls
{
	[TestClass]
	public class CanvasTest
	{
		[TestMethod]
		public void ChildlessMeasureTest ()
		{
			Canvas c = new Canvas ();
			Size s = new Size (10,10);

			c.Measure (s);

			Assert.AreEqual (new Size (0,0), c.DesiredSize, "DesiredSize");
		}

		[TestMethod]
		public void ChildMeasureTest1 ()
		{
			Canvas c = new Canvas ();
			Rectangle r = new Rectangle();

			c.Children.Add (r);

			Canvas.SetLeft (r, 10);
			Canvas.SetTop (r, 10);

			r.Width = 50;
			r.Height = 50;

			c.Measure (new Size (10, 10));

			Assert.AreEqual (new Size (0,0), c.DesiredSize);
		}

		[TestMethod]
		public void ChildMeasureTest2 ()
		{
			Canvas c = new Canvas ();
			Rectangle r = new Rectangle();

			c.Children.Add (r);

			Canvas.SetLeft (r, 10);
			Canvas.SetTop (r, 10);

			r.Width = 50;
			r.Height = 50;

			c.Measure (new Size (100, 100));

			Assert.AreEqual (new Size (0,0), c.DesiredSize);
		}

		[TestMethod]
		public void ChildMeasureTest3 ()
		{
			Canvas c = new Canvas ();
			Border r = new Border ();

			c.Children.Add (r);

			Canvas.SetLeft (r, 10);
			Canvas.SetTop (r, 10);

			r.Width = 50;
			r.Height = 50;

			c.Measure (new Size (100, 100));

			Assert.AreEqual (new Size (0,0), c.DesiredSize);
			Assert.AreEqual (new Size (0,0), r.DesiredSize);
		}

		[TestMethod]
		public void ChildMeasureTest4 ()
		{
			Border root = new Border ();
			Canvas c = new Canvas ();
			Border r = new Border ();

			root.Child = c;
			c.Children.Add (r);

			Canvas.SetLeft (r, 10);
			Canvas.SetTop (r, 10);

			r.Width = 50;
			r.Height = 50;

			c.Measure (new Size (100, 100));

			Assert.AreEqual (new Size (0,0), c.DesiredSize);
			Assert.AreEqual (new Size (50,50), r.DesiredSize);
		}

		[TestMethod]
		public void ChildBackgroundMeasureTest1 ()
		{
			Canvas c = new Canvas ();
			Rectangle r = new Rectangle();

			c.Background = new SolidColorBrush (Colors.Black);
			c.Children.Add (r);

			Canvas.SetLeft (r, 10);
			Canvas.SetTop (r, 10);

			r.Width = 50;
			r.Height = 50;

			c.Measure (new Size (10, 10));

			Assert.AreEqual (new Size (0,0), c.DesiredSize);
		}

		[TestMethod]
		public void ChildBackgroundMeasureTest2 ()
		{
			Canvas c = new Canvas ();
			Rectangle r = new Rectangle();

			c.Background = new SolidColorBrush (Colors.Black);
			c.Children.Add (r);

			Canvas.SetLeft (r, 10);
			Canvas.SetTop (r, 10);

			r.Width = 50;
			r.Height = 50;

			c.Measure (new Size (100, 100));

			Assert.AreEqual (new Size (0,0), c.DesiredSize);
		}

		[TestMethod]
		public void WidthHeightMeasureTest1 ()
		{
			Canvas c = new Canvas ();

			c.Width = 50;
			c.Height = 60;

			c.Measure (new Size (10, 10));

			Assert.AreEqual (new Size (0,0), c.DesiredSize);
		}

		[TestMethod]
		public void WidthHeightMeasureTest2 ()
		{
			Canvas c = new Canvas ();

			c.Width = 50;
			c.Height = 60;

			c.Measure (new Size (100, 100));

			Assert.AreEqual (new Size (0,0), c.DesiredSize);
		}
	
		[TestMethod]
		public void WidthHeightBackgroundMeasureTest1 ()
		{
			Canvas c = new Canvas ();

			c.Background = new SolidColorBrush (Colors.Black);

			c.Width = 50;
			c.Height = 60;

			c.Measure (new Size (10, 10));

			Assert.AreEqual (new Size (0,0), c.DesiredSize);
		}

		[TestMethod]
		public void WidthHeightBackgroundMeasureTest2 ()
		{
			Canvas c = new Canvas ();

			c.Background = new SolidColorBrush (Colors.Black);

			c.Width = 50;
			c.Height = 60;

			c.Measure (new Size (100, 100));

			Assert.AreEqual (new Size (0,0), c.DesiredSize);
		}

		[TestMethod]
		public void ChildNameScope ()
		{
			Canvas b = new Canvas ();
		        Canvas c = (Canvas)XamlReader.Load (@"
<Canvas xmlns=""http://schemas.microsoft.com/client/2007"" xmlns:x=""http://schemas.microsoft.com/winfx/2006/xaml"">
  <Border>
    <Path x:Name=""foo"" Data=""F1 M 10,10 20,20 10,20"" Stroke=""Red""/>
  </Border>
</Canvas>");
			Assert.IsNotNull (c.FindName ("foo"));
			
			b.Children.Add (c);
			
			Assert.IsNull (b.FindName ("foo"));
			Assert.IsNotNull (c.FindName ("foo"));
		}
	}

}
