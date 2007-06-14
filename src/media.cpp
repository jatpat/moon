/*
 * media.cpp: 
 *
 * Author: Jeffrey Stedfast <fejj@novell.com>
 *
 * Copyright 2007 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#define Visual _XVisual
#include <gdk/gdkx.h>
#include <cairo-xlib.h>
#undef Visual

#include "media.h"

// MediaBase

DependencyProperty *MediaBase::SourceProperty;
DependencyProperty *MediaBase::StretchProperty;

char *
media_base_get_source (MediaBase *media)
{
	Value *value = media->GetValue (MediaBase::SourceProperty);
	
	return value ? (char *) value->AsString () : NULL;
}

void
media_base_set_source (MediaBase *media, char *value)
{
	media->SetValue (MediaBase::SourceProperty, Value (value));
}

Stretch
media_base_get_stretch (MediaBase *media)
{
	return (Stretch) media->GetValue (MediaBase::StretchProperty)->AsInt32 ();
}

void
media_base_set_stretch (MediaBase *media, Stretch value)
{
	media->SetValue (MediaBase::StretchProperty, Value (value));
}



// MediaElement

DependencyProperty *MediaElement::AutoPlayProperty;
DependencyProperty *MediaElement::BalanceProperty;
DependencyProperty *MediaElement::BufferingProgressProperty;
DependencyProperty *MediaElement::BufferingTimeProperty;
DependencyProperty *MediaElement::CanSeekProperty;
DependencyProperty *MediaElement::CurrentStateProperty;
DependencyProperty *MediaElement::DownloadProgressProperty;
DependencyProperty *MediaElement::IsMutedProperty;
DependencyProperty *MediaElement::MarkersProperty;
DependencyProperty *MediaElement::NaturalDurationProperty;
DependencyProperty *MediaElement::NaturalVideoHeightProperty;
DependencyProperty *MediaElement::NaturalVideoWidthProperty;
DependencyProperty *MediaElement::PositionProperty;
DependencyProperty *MediaElement::VolumeProperty;


void
MediaElement::SetSource (DependencyObject *downloader, char *name)
{
	;
}

void
MediaElement::Pause ()
{
	;
}

void
MediaElement::Play ()
{
	;
}

void
MediaElement::Stop ()
{
	;
}

MediaElement *
media_element_new ()
{
	return new MediaElement ();
}

bool
media_element_get_auto_play (MediaElement *media)
{
	return (bool) media->GetValue (MediaElement::AutoPlayProperty)->AsBool ();
}

void
media_element_set_auto_play (MediaElement *media, bool value)
{
	media->SetValue (MediaElement::AutoPlayProperty, Value (value));
}

double
media_element_get_balance (MediaElement *media)
{
	return (double) media->GetValue (MediaElement::BalanceProperty)->AsDouble ();
}

void
media_element_set_balance (MediaElement *media, double value)
{
	media->SetValue (MediaElement::BalanceProperty, Value (value));
}

double
media_element_get_buffering_progress (MediaElement *media)
{
	return (double) media->GetValue (MediaElement::BufferingProgressProperty)->AsDouble ();
}

void
media_element_set_buffering_progress (MediaElement *media, double value)
{
	media->SetValue (MediaElement::BufferingProgressProperty, Value (value));
}

TimeSpan
media_element_get_buffering_time (MediaElement *media)
{
	return (TimeSpan) media->GetValue (MediaElement::BufferingTimeProperty)->AsInt64 ();
}

void
media_element_set_buffering_time (MediaElement *media, TimeSpan value)
{
	media->SetValue (MediaElement::BufferingTimeProperty, Value (value));
}

bool
media_element_get_can_seek (MediaElement *media)
{
	return (bool) media->GetValue (MediaElement::CanSeekProperty)->AsBool ();
}

char *
media_element_get_current_state (MediaElement *media)
{
	Value *value = media->GetValue (MediaElement::CurrentStateProperty);
	
	return value ? (char *) value->AsString () : NULL;
}

double
media_element_get_download_progress (MediaElement *media)
{
	return (double) media->GetValue (MediaElement::DownloadProgressProperty)->AsDouble ();
}

void
media_element_set_download_progress (MediaElement *media, double value)
{
	media->SetValue (MediaElement::DownloadProgressProperty, Value (value));
}

bool
media_element_get_is_muted (MediaElement *media)
{
	return (bool) media->GetValue (MediaElement::IsMutedProperty)->AsBool ();
}

void
media_element_set_is_muted (MediaElement *media, bool value)
{
	media->SetValue (MediaElement::IsMutedProperty, Value (value));
}

TimelineMarkerCollection *
media_element_get_markers (MediaElement *media)
{
	Value *value = media->GetValue (MediaElement::MarkersProperty);
	
	return value ? (TimelineMarkerCollection *) value->AsTimelineMarkerCollection () : NULL;
}

void
media_element_set_markers (MediaElement *media, TimelineMarkerCollection *value)
{
	TimelineMarkerCollection *markers = media_element_get_markers (media);
	
	if (markers != NULL)
		base_unref (markers);
	
	base_ref (value);
	
	media->SetValue (MediaElement::MarkersProperty, Value (value));
}

Duration *
media_element_get_natural_duration (MediaElement *media)
{
	return (Duration *) media->GetValue (MediaElement::NaturalDurationProperty)->AsDuration ();
}

void
media_element_set_natural_duration (MediaElement *media, Duration value)
{
	media->SetValue (MediaElement::NaturalDurationProperty, Value (value));
}

double
media_element_get_natural_video_height (MediaElement *media)
{
	return (double) media->GetValue (MediaElement::NaturalVideoHeightProperty)->AsDouble ();
}

void
media_element_set_natural_video_height (MediaElement *media, double value)
{
	media->SetValue (MediaElement::NaturalVideoHeightProperty, Value (value));
}

double
media_element_get_natural_video_width (MediaElement *media)
{
	return (double) media->GetValue (MediaElement::NaturalVideoWidthProperty)->AsDouble ();
}

void
media_element_set_natural_video_width (MediaElement *media, double value)
{
	media->SetValue (MediaElement::NaturalVideoWidthProperty, Value (value));
}

TimeSpan
media_element_get_position (MediaElement *media)
{
	return (TimeSpan) media->GetValue (MediaElement::PositionProperty)->AsInt64 ();
}

void
media_element_set_position (MediaElement *media, TimeSpan value)
{
	media->SetValue (MediaElement::PositionProperty, Value (value));
}

double
media_element_get_volume (MediaElement *media)
{
	return (double) media->GetValue (MediaElement::VolumeProperty)->AsDouble ();
}

void
media_element_set_volume (MediaElement *media, double value)
{
	media->SetValue (MediaElement::VolumeProperty, Value (value));
}

//
// Image
//
DependencyProperty* Image::DownloadProgressProperty;

Image::Image ()
  : pixbuf_width (0),
    pixbuf_height (0),
    loader (NULL),
    xlib_surface (NULL)
{
}

void
Image::SetSource (DependencyObject *downloader, char* PartName)
{
	loader = gdk_pixbuf_loader_new ();

	g_signal_connect (loader, "size_prepared", G_CALLBACK(loader_size_prepared), this);

	this->downloader = downloader;
	((Downloader*)downloader)->SetWriteFunc (pixbuf_write, this);
	((Downloader*)downloader)->Open ("GET", PartName, true);
}

void
Image::pixbuf_write (guchar *buf, gsize count, gpointer data)
{
	((Image*)data)->PixbufWrite (buf, count);
}

void
Image::PixbufWrite (guchar *buf, gsize count)
{
	gdk_pixbuf_loader_write (loader, buf, count, NULL);
}

void
Image::loader_size_prepared (GdkPixbufLoader *loader, int width, int height, gpointer data)
{
	((Image*)data)->LoaderSizePrepared (width, height);
}

void
Image::LoaderSizePrepared (int width, int height)
{
	printf ("image has size %dx%d\n", width, height);
	pixbuf_width = width;
	pixbuf_height = height;

	item_update_bounds (this);
}

void
Image::render (Surface *s, int x, int y, int width, int height)
{
	cairo_save (s->cairo);

	GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
	if (pixbuf) {
		if (!xlib_surface) {
			pixmap = gdk_pixmap_new (GDK_DRAWABLE (s->drawing_area->window),
						 gdk_pixbuf_get_width (pixbuf),
						 gdk_pixbuf_get_height (pixbuf),
						 gdk_drawable_get_depth (GDK_DRAWABLE (s->drawing_area->window)));
			GdkGC *gc = gdk_gc_new (GDK_DRAWABLE (pixmap));
			gdk_draw_pixbuf (GDK_DRAWABLE (pixmap),
					 gc,
					 pixbuf,
					 0, 0,
					 0, 0,
					 gdk_pixbuf_get_width (pixbuf),
					 gdk_pixbuf_get_height (pixbuf),
					 GDK_RGB_DITHER_NONE,
					 0,0);
			g_object_unref (G_OBJECT (gc));
			

			xlib_surface = cairo_xlib_surface_create (GDK_PIXMAP_XDISPLAY (pixmap),
								  GDK_PIXMAP_XID (pixmap),
								  GDK_VISUAL_XVISUAL (gdk_drawable_get_visual (GDK_DRAWABLE (pixmap))),
								  gdk_pixbuf_get_width (pixbuf),
								  gdk_pixbuf_get_height (pixbuf));
		}

		cairo_set_matrix (s->cairo, &absolute_xform);
	
		cairo_set_source_surface (s->cairo, xlib_surface, 0, 0);

		cairo_rectangle (s->cairo, 0, 0, this->pixbuf_width, this->pixbuf_height);

		cairo_fill (s->cairo);
	}
	cairo_restore (s->cairo);
}

void
Image::getbounds ()
{
	x1 = y1 = 0;
	x2 = pixbuf_width;
	y2 = pixbuf_height;
}

Image*
image_new ()
{
	return new Image ();
}

void
image_set_download_progress (Image *img, double progress)
{
	img->SetValue (Image::DownloadProgressProperty, Value(progress));
}

double
image_get_download_progress (Image *img)
{
	return img->GetValue (Image::DownloadProgressProperty)->AsDouble();
}

void
image_set_source (Image *img, DependencyObject *Downloader, char *PartName)
{
	img->SetSource (Downloader, PartName);
}

//
// MediaAttribute
//

DependencyProperty* MediaAttribute::ValueProperty;

MediaAttribute*
media_attribute_new ()
{
	return new MediaAttribute ();
}

void
media_init ()
{
	/* MediaAttribute */
	MediaAttribute::ValueProperty = DependencyObject::Register (Value::MEDIAATTRIBUTE, "Value", new Value (""));
	
	/* MediaBase */
	MediaBase::SourceProperty = DependencyObject::Register (Value::MEDIABASE, "Source", Value::STRING);
	MediaBase::StretchProperty = DependencyObject::Register (Value::MEDIABASE, "Stretch", new Value (StretchNone));
	
	/* Image */
	Image::DownloadProgressProperty = DependencyObject::Register (Value::IMAGE, "DownloadProgress", new Value (0.0));
	
	/* MediaElement */
	MediaElement::AutoPlayProperty = DependencyObject::Register (Value::MEDIAELEMENT, "AutoPlay", new Value (true));
	MediaElement::BalanceProperty = DependencyObject::Register (Value::MEDIAELEMENT, "Balance", new Value (0.0));
	MediaElement::BufferingProgressProperty = DependencyObject::Register (Value::MEDIAELEMENT, "BufferingProgress", new Value (0.0));
	MediaElement::BufferingTimeProperty = DependencyObject::Register (Value::MEDIAELEMENT, "BufferingTime", new Value (0));
	MediaElement::CanSeekProperty = DependencyObject::Register (Value::MEDIAELEMENT, "CanSeek", new Value (false));
	MediaElement::CurrentStateProperty = DependencyObject::Register (Value::MEDIAELEMENT, "CurrentState", Value::STRING);
	MediaElement::DownloadProgressProperty = DependencyObject::Register (Value::MEDIAELEMENT, "DownloadProgress", new Value (0.0));
	MediaElement::IsMutedProperty = DependencyObject::Register (Value::MEDIAELEMENT, "IsMuted", new Value (false));
	MediaElement::MarkersProperty = DependencyObject::Register (Value::MEDIAELEMENT, "Markers", Value::TIMELINEMARKER_COLLECTION);
	MediaElement::NaturalDurationProperty = DependencyObject::Register (Value::MEDIAELEMENT, "NaturalDuration", Value::DURATION);
	MediaElement::NaturalVideoHeightProperty = DependencyObject::Register (Value::MEDIAELEMENT, "NaturalVideoHeight", Value::DOUBLE);
	MediaElement::NaturalVideoWidthProperty = DependencyObject::Register (Value::MEDIAELEMENT, "NaturalVideoWidth", Value::DOUBLE);
	MediaElement::PositionProperty = DependencyObject::Register (Value::MEDIAELEMENT, "Position", Value::DOUBLE);
	MediaElement::VolumeProperty = DependencyObject::Register (Value::MEDIAELEMENT, "Volume", Value::DOUBLE);
}
