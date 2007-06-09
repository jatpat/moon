/*
 * geometry.cpp: Geometry classes
 *
 * Author:
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright 2007 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 * 
 */

#include <config.h>
#include <string.h>
#include <gtk/gtk.h>
#include <malloc.h>
#include <glib.h>
#include <stdlib.h>

#include "geometry.h"
#include "shape.h"

//
// Geometry
//

DependencyProperty* Geometry::FillRuleProperty;
DependencyProperty* Geometry::TransformProperty;

FillRule
geometry_get_fill_rule (Geometry *geometry)
{
	return (FillRule) geometry->GetValue (Geometry::FillRuleProperty)->AsInt32();
}

void
geometry_set_fill_rule (Geometry *geometry, FillRule fill_rule)
{
	geometry->SetValue (Geometry::FillRuleProperty, Value (fill_rule));
}

Transform*
geometry_get_transform (Geometry *geometry)
{
	Value *value = geometry->GetValue (Geometry::TransformProperty);
	return value ? value->AsTransform() : NULL;
}

void
geometry_set_transform (Geometry *geometry, Transform *transform)
{
	geometry->SetValue (Geometry::TransformProperty, Value (transform));
}

void
Geometry::Draw (Surface *s)
{
	cairo_set_fill_rule (s->cairo, convert_fill_rule (geometry_get_fill_rule (this)));
	Transform* transform = geometry_get_transform (this);
	if (transform) {
		cairo_matrix_t matrix;
		transform->GetTransform (&matrix);
		cairo_transform (s->cairo, &matrix);
	}
}

//
// GeometryGroup
//

DependencyProperty* GeometryGroup::ChildrenProperty;

GeometryGroup*
geometry_group_new ()
{
	return new GeometryGroup ();
}

GeometryGroup::GeometryGroup ()
{
	children = NULL;
	GeometryCollection *c = new GeometryCollection ();

	this->SetValue (GeometryGroup::ChildrenProperty, Value (c));

	// Ensure that the callback OnPropertyChanged was called.
	g_assert (c == children);
}

void
GeometryGroup::OnPropertyChanged (DependencyProperty *prop)
{
	Geometry::OnPropertyChanged (prop);

	if (prop == ChildrenProperty){
		// The new value has already been set, so unref the old collection

		GeometryCollection *newcol = GetValue (prop)->AsGeometryCollection();

		if (newcol != children){
			if (children){
				for (GSList *l = children->list; l != NULL; l = l->next){
					DependencyObject *dob = (DependencyObject *) l->data;
					
					base_unref (dob);
				}
				base_unref (children);
				g_slist_free (children->list);
			}

			children = newcol;
			if (children->closure)
				printf ("Warning we attached a property that was already attached\n");
			children->closure = this;
			
			base_ref (children);
		}
	}
}

void
GeometryGroup::Draw (Surface *s)
{
	Geometry::Draw (s);

	for (GSList *g = children->list; g != NULL; g = g->next) {
		Geometry *geometry = (Geometry*) g->data;
		geometry->Draw (s);
	}
}

GeometryCollection*
geometry_group_get_children (GeometryGroup *geometry_group)
{
	Value *value = geometry_group->GetValue (GeometryGroup::ChildrenProperty);
	return (GeometryCollection*) (value ? value->AsGeometryCollection() : NULL);
}

void
geometry_group_set_children (GeometryGroup *geometry_group, GeometryCollection* geometry_collection)
{
	geometry_group->SetValue (GeometryGroup::ChildrenProperty, Value (geometry_collection));
}

//
// GeometryCollection
//

void
GeometryCollection::Add (void *data)
{
	Value *value = (Value*) data;
	Geometry *geometry = value->AsGeometry ();
	Collection::Add (geometry);
}

void
GeometryCollection::Remove (void *data)
{
	Value *value = (Value*) data;
	Geometry *geometry = value->AsGeometry ();
	Collection::Remove (geometry);
}

//
// EllipseGeometry
//

DependencyProperty* EllipseGeometry::CenterProperty;
DependencyProperty* EllipseGeometry::RadiusXProperty;
DependencyProperty* EllipseGeometry::RadiusYProperty;

Point*
ellipse_geometry_get_center (EllipseGeometry *ellipse_geometry)
{
	Value *value = ellipse_geometry->GetValue (EllipseGeometry::CenterProperty);
	return (value ? value->AsPoint() : NULL);
}

void
ellipse_geometry_set_center (EllipseGeometry *ellipse_geometry, Point *point)
{
	ellipse_geometry->SetValue (EllipseGeometry::CenterProperty, Value (*point));
}

double
ellipse_geometry_get_radius_x (EllipseGeometry *ellipse_geometry)
{
	return ellipse_geometry->GetValue (EllipseGeometry::RadiusXProperty)->AsDouble();
}

void
ellipse_geometry_set_radius_x (EllipseGeometry *ellipse_geometry, double radius_x)
{
	ellipse_geometry->SetValue (EllipseGeometry::RadiusXProperty, Value (radius_x));
}

double
ellipse_geometry_get_radius_y (EllipseGeometry *ellipse_geometry)
{
	return ellipse_geometry->GetValue (EllipseGeometry::RadiusYProperty)->AsDouble();
}

void
ellipse_geometry_set_radius_y (EllipseGeometry *ellipse_geometry, double radius_y)
{
	ellipse_geometry->SetValue (EllipseGeometry::RadiusYProperty, Value (radius_y));
}

EllipseGeometry*
ellipse_geometry_new ()
{
	return new EllipseGeometry ();
}

void
EllipseGeometry::Draw (Surface *s)
{
	Geometry::Draw (s);

	Point *pt = ellipse_geometry_get_center (this);
	double rx = ellipse_geometry_get_radius_x (this);
	double ry = ellipse_geometry_get_radius_y (this);

	moon_ellipse (s->cairo, pt->x - rx, pt->y - ry, rx * 2, ry * 2);
}

//
// LineGeometry
//

DependencyProperty* LineGeometry::EndPointProperty;
DependencyProperty* LineGeometry::StartPointProperty;

Point*
line_geometry_get_end_point (LineGeometry* line_geometry)
{
	Value *value = line_geometry->GetValue (LineGeometry::EndPointProperty);
	return (value ? value->AsPoint() : new Point ());
}

void
line_geometry_set_end_point (LineGeometry* line_geometry, Point *end_point)
{
	line_geometry->SetValue (LineGeometry::EndPointProperty, Value (*end_point));
}

Point*
line_geometry_get_start_point (LineGeometry* line_geometry)
{
	Value *value = line_geometry->GetValue (LineGeometry::StartPointProperty);
	return (value ? value->AsPoint() : new Point ());
}

void
line_geometry_set_start_point (LineGeometry* line_geometry, Point *start_point)
{
	line_geometry->SetValue (LineGeometry::StartPointProperty, Value (*start_point));
}

LineGeometry*
line_geometry_new ()
{
	return new LineGeometry ();
}

void
LineGeometry::Draw (Surface *s)
{
	Geometry::Draw (s);

	Point *p1 = line_geometry_get_start_point (this);
	Point *p2 = line_geometry_get_end_point (this);

	cairo_move_to (s->cairo, p1->x, p1->y);
	cairo_line_to (s->cairo, p2->x, p2->y);
}

//
// PathGeometry
//

DependencyProperty* PathGeometry::FiguresProperty;

PathGeometry*
path_geometry_new ()
{
	return new PathGeometry ();
}

void
PathGeometry::Draw (Surface *s)
{
	Geometry::Draw (s);
	// TODO, iterates all figures (and their segments) to build the path
}

//
// RectangleGeometry
//

DependencyProperty* RectangleGeometry::RadiusXProperty;
DependencyProperty* RectangleGeometry::RadiusYProperty;
DependencyProperty* RectangleGeometry::RectProperty;

double
rectangle_geometry_get_radius_x (RectangleGeometry *rectangle_geometry)
{
	return rectangle_geometry->GetValue (RectangleGeometry::RadiusXProperty)->AsDouble();
}

void
rectangle_geometry_set_radius_x (RectangleGeometry *rectangle_geometry, double radius_x)
{
	rectangle_geometry->SetValue (RectangleGeometry::RadiusXProperty, Value (radius_x));
}

double
rectangle_geometry_get_radius_y (RectangleGeometry *rectangle_geometry)
{
	return rectangle_geometry->GetValue (RectangleGeometry::RadiusYProperty)->AsDouble();
}

void
geometry_set_radius_y (RectangleGeometry *rectangle_geometry, double radius_y)
{
	rectangle_geometry->SetValue (RectangleGeometry::RadiusYProperty, Value (radius_y));
}

Rect*
rectangle_geometry_get_rect (RectangleGeometry *rectangle_geometry)
{
	Value *value = rectangle_geometry->GetValue (RectangleGeometry::RectProperty);
	return (value ? value->AsRect() : NULL);
}

void
rectangle_geometry_set_rect (RectangleGeometry *rectangle_geometry, Rect *rect)
{
	rectangle_geometry->SetValue (RectangleGeometry::RectProperty, Value (*rect));
}

RectangleGeometry*
rectangle_geometry_new ()
{
	return new RectangleGeometry ();
}

void
RectangleGeometry::Draw (Surface *s)
{
	Geometry::Draw (s);

	Rect *rect = rectangle_geometry_get_rect (this);
	double radius_x = rectangle_geometry_get_radius_x  (this);
	if (radius_x != 0) {
		double radius_y = rectangle_geometry_get_radius_y (this);
		if (radius_y != 0) {
			moon_rounded_rectangle (s->cairo, rect->x, rect->y, rect->w, rect->h, radius_x, radius_y);
			return;
		}
	}
	// normal rectangle
	cairo_rectangle (s->cairo, rect->x, rect->y, rect->w, rect->h);
}

//
// PathFigure
//

DependencyProperty* PathFigure::IsClosedProperty;
DependencyProperty* PathFigure::IsFilledProperty;
DependencyProperty* PathFigure::SegmentsProperty;
DependencyProperty* PathFigure::StartPointProperty;

PathFigure*
path_figure_new ()
{
	return new PathFigure ();
}

bool
path_figure_get_is_closed (PathFigure *path_figure)
{
	return path_figure->GetValue (PathFigure::IsClosedProperty)->AsBool();
}

void
path_figure_set_is_closed (PathFigure *path_figure, bool closed)
{
	path_figure->SetValue (PathFigure::IsClosedProperty, Value (closed));
}

bool
path_figure_get_is_filled (PathFigure *path_figure)
{
	return path_figure->GetValue (PathFigure::IsFilledProperty)->AsBool();
}

void
path_figure_set_is_filled (PathFigure *path_figure, bool filled)
{
	path_figure->SetValue (PathFigure::IsFilledProperty, Value (filled));
}

// TODO Segments

Point*
path_figure_get_start_point (PathFigure *path_figure)
{
	Value *value = path_figure->GetValue (PathFigure::StartPointProperty);
	return (value ? value->AsPoint() : new Point (0, 0));
}

void
path_figure_set_start_point (PathFigure *path_figure, Point *point)
{
	path_figure->SetValue (PathFigure::StartPointProperty, Value (*point));
}

void
PathFigure::Draw (Surface *s)
{
	Point *start = path_figure_get_start_point (this);

	// should not be required because of the cairo_move_to
	//cairo_new_sub_path (s->cairo);
	cairo_move_to (s->cairo, start->x, start->y);

	// TODO: iterates segments

	if (path_figure_get_is_closed (this)) {
		cairo_close_path (s->cairo);
	}

	if (path_figure_get_is_filled (this)) {
		// FIXME: fill is setup in Shape::DoDraw but shouldn't be always called
	}
}

//
// ArcSegment
//

DependencyProperty* ArcSegment::IsLargeArcProperty;
DependencyProperty* ArcSegment::PointProperty;
DependencyProperty* ArcSegment::RotationAngleProperty;
DependencyProperty* ArcSegment::SizeProperty;
DependencyProperty* ArcSegment::SweepDirectionProperty;

ArcSegment*
arc_segment_new ()
{
	return new ArcSegment ();
}

bool
arc_segment_get_is_large_arc (ArcSegment *segment)
{
	return segment->GetValue (ArcSegment::IsLargeArcProperty)->AsBool();
}

void
arc_segment_set_is_large_arc (ArcSegment *segment, bool large)
{
	segment->SetValue (ArcSegment::IsLargeArcProperty, Value (large));
}

Point*
arc_segment_get_point (ArcSegment *segment)
{
	Value *value = segment->GetValue (ArcSegment::PointProperty);
	return (value ? value->AsPoint() : NULL);
}

void
arc_segment_set_point (ArcSegment *segment, Point *point)
{
	segment->SetValue (ArcSegment::PointProperty, Value (*point));
}

double
arc_segment_get_rotation_angle (ArcSegment *segment)
{
	return segment->GetValue (ArcSegment::RotationAngleProperty)->AsDouble();
}

void
arc_segment_set_rotation_angle (ArcSegment *segment, double angle)
{
	segment->SetValue (ArcSegment::RotationAngleProperty, Value (angle));
}

Point*
arc_segment_get_size (ArcSegment *segment)
{
	Value *value = segment->GetValue (ArcSegment::SizeProperty);
	return (value ? value->AsPoint() : NULL);
}

void
arc_segment_set_size (ArcSegment *segment, Point *size)
{
	segment->SetValue (ArcSegment::SizeProperty, Value (*size));
}

SweepDirection
arc_segment_get_sweep_direction (ArcSegment *segment)
{
	return (SweepDirection) segment->GetValue (ArcSegment::SweepDirectionProperty)->AsInt32();
}

void
arc_segment_set_sweep_direction (ArcSegment *segment, SweepDirection direction)
{
	segment->SetValue (ArcSegment::SweepDirectionProperty, Value (direction));
}

//
// BezierSegment
//

DependencyProperty* BezierSegment::Point1Property;
DependencyProperty* BezierSegment::Point2Property;
DependencyProperty* BezierSegment::Point3Property;

BezierSegment*
bezier_segment_new ()
{
	return new BezierSegment ();
}

Point*
bezier_segment_get_point1 (BezierSegment *segment)
{
	Value *value = segment->GetValue (BezierSegment::Point1Property);
	return (value ? value->AsPoint() : NULL);
}

void
bezier_segment_set_point1 (BezierSegment *segment, Point *point)
{
	segment->SetValue (BezierSegment::Point1Property, Value (*point));
}

Point*
bezier_segment_get_point2 (BezierSegment *segment)
{
	Value *value = segment->GetValue (BezierSegment::Point2Property);
	return (value ? value->AsPoint() : NULL);
}

void
bezier_segment_set_point2 (BezierSegment *segment, Point *point)
{
	segment->SetValue (BezierSegment::Point2Property, Value (*point));
}

Point*
bezier_segment_get_point3 (BezierSegment *segment)
{
	Value *value = segment->GetValue (BezierSegment::Point3Property);
	return (value ? value->AsPoint() : NULL);
}

void
bezier_segment_set_point3 (BezierSegment *segment, Point *point)
{
	segment->SetValue (BezierSegment::Point3Property, Value (*point));
}

//
// LineSegment
//

DependencyProperty* LineSegment::PointProperty;

LineSegment*
line_segment_new ()
{
	return new LineSegment ();
}

Point*
line_segment_get_point (LineSegment *segment)
{
	Value *value = segment->GetValue (LineSegment::PointProperty);
	return (value ? value->AsPoint() : NULL);
}

void
line_segment_set_point (LineSegment *segment, Point *point)
{
	segment->SetValue (LineSegment::PointProperty, Value (*point));
}

//
// PolyBezierSegment
//

DependencyProperty* PolyBezierSegment::PointsProperty;

PolyBezierSegment*
poly_bezier_segment_new ()
{
	return new PolyBezierSegment ();
}

/*
 * note: We return a reference, not a copy, of the points. Not a big issue as
 * Silverlight PolyBezierSegment.Points only has a setter (no getter), so it's
 * use is only internal.
 */
Point*
poly_bezier_segment_get_points (PolyBezierSegment *segment, int *count)
{
	Value *value = segment->GetValue (PolyBezierSegment::PointsProperty);
	if (!value) {
		*count = 0;
		return NULL;
	}

	PointArray *pa = value->AsPointArray();
	*count = pa->count;
	return pa->points;
}

void
poly_bezier_segment_set_points (PolyBezierSegment *segment, Point *points, int count)
{
	segment->SetValue (PolyBezierSegment::PointsProperty, Value (points, count));
}

//
// PolyLineSegment
//

DependencyProperty* PolyLineSegment::PointsProperty;

PolyLineSegment*
poly_line_segment_new ()
{
	return new PolyLineSegment ();
}

/*
 * note: We return a reference, not a copy, of the points. Not a big issue as
 * Silverlight PolyLineSegment.Points only has a setter (no getter), so it's
 * use is only internal.
 */
Point*
poly_line_segment_get_points (PolyLineSegment *segment, int *count)
{
	Value *value = segment->GetValue (PolyLineSegment::PointsProperty);
	if (!value) {
		*count = 0;
		return NULL;
	}

	PointArray *pa = value->AsPointArray();
	*count = pa->count;
	return pa->points;
}

void
poly_line_segment_set_points (PolyLineSegment *segment, Point *points, int count)
{
	segment->SetValue (PolyLineSegment::PointsProperty, Value (points, count));
}

//
// PolyQuadraticBezierSegment
//

DependencyProperty* PolyQuadraticBezierSegment::PointsProperty;

PolyQuadraticBezierSegment*
poly_quadratic_segment_new ()
{
	return new PolyQuadraticBezierSegment ();
}

/*
 * note: We return a reference, not a copy, of the points. Not a big issue as
 * Silverlight PolyQuadraticBezierSegment.Points only has a setter (no getter),
 * so it's use is only internal.
 */
Point*
poly_quadratic_segment_get_points (PolyQuadraticBezierSegment *segment, int *count)
{
	Value *value = segment->GetValue (PolyQuadraticBezierSegment::PointsProperty);
	if (!value) {
		*count = 0;
		return NULL;
	}

	PointArray *pa = value->AsPointArray();
	*count = pa->count;
	return pa->points;
}

void
poly_quadratic_segment_set_points (PolyQuadraticBezierSegment *segment, Point *points, int count)
{
	segment->SetValue (PolyQuadraticBezierSegment::PointsProperty, Value (points, count));
}

//
// QuadraticBezierSegment
//

DependencyProperty* QuadraticBezierSegment::Point1Property;
DependencyProperty* QuadraticBezierSegment::Point2Property;

QuadraticBezierSegment*
quadratic_bezier_segment_new ()
{
	return new QuadraticBezierSegment ();
}

Point*
quadratic_bezier_segment_get_point1 (QuadraticBezierSegment *segment)
{
	Value *value = segment->GetValue (QuadraticBezierSegment::Point1Property);
	return (value ? value->AsPoint() : NULL);
}

void
quadratic_bezier_segment_set_point1 (QuadraticBezierSegment *segment, Point *point)
{
	segment->SetValue (QuadraticBezierSegment::Point1Property, Value (*point));
}

Point*
quadratic_bezier_segment_get_point2 (QuadraticBezierSegment *segment)
{
	Value *value = segment->GetValue (QuadraticBezierSegment::Point2Property);
	return (value ? value->AsPoint() : NULL);
}

void
quadratic_bezier_segment_set_point2 (QuadraticBezierSegment *segment, Point *point)
{
	segment->SetValue (QuadraticBezierSegment::Point2Property, Value (*point));
}

//
// 
//

void
geometry_init ()
{
	/* Geometry fields */
	Geometry::FillRuleProperty = DependencyObject::Register (Value::GEOMETRY, "FillRule", new Value (FillRuleEvenOdd));
	Geometry::TransformProperty = DependencyObject::Register (Value::GEOMETRY, "Transform", Value::TRANSFORM);

	/* GeometryGroup fields */
	GeometryGroup::ChildrenProperty = DependencyObject::Register (Value::GEOMETRYGROUP, "Children", Value::DEPENDENCY_OBJECT);

	/* EllipseGeometry fields */
	EllipseGeometry::CenterProperty = DependencyObject::Register (Value::ELLIPSEGEOMETRY, "Center", Value::POINT);
	EllipseGeometry::RadiusXProperty = DependencyObject::Register (Value::ELLIPSEGEOMETRY, "RadiusX", new Value (0.0));
	EllipseGeometry::RadiusYProperty = DependencyObject::Register (Value::ELLIPSEGEOMETRY, "RadiusY", new Value (0.0));

	/* LineGeometry fields */
	LineGeometry::EndPointProperty = DependencyObject::Register (Value::LINEGEOMETRY, "EndPoint", Value::POINT);
	LineGeometry::StartPointProperty = DependencyObject::Register (Value::LINEGEOMETRY, "StartPoint", Value::POINT);

	/* PathGeometry */
	PathGeometry::FiguresProperty = DependencyObject::Register (Value::PATHGEOMETRY, "Figures", Value::DEPENDENCY_OBJECT);

	/* RectangleGeometry fields */
	RectangleGeometry::RadiusXProperty = DependencyObject::Register (Value::RECTANGLEGEOMETRY, "RadiusX", new Value (0.0));
	RectangleGeometry::RadiusYProperty = DependencyObject::Register (Value::RECTANGLEGEOMETRY, "RadiusY", new Value (0.0));
	RectangleGeometry::RectProperty = DependencyObject::Register (Value::RECTANGLEGEOMETRY, "Rect", Value::RECT);

	/* PathFigure fields */
	PathFigure::IsClosedProperty = DependencyObject::Register (Value::PATHFIGURE, "IsClosed", new Value (false));
	PathFigure::IsFilledProperty = DependencyObject::Register (Value::PATHFIGURE, "IsFilled", new Value (true));
	PathFigure::SegmentsProperty = DependencyObject::Register (Value::PATHFIGURE, "Segments", Value::DEPENDENCY_OBJECT);
	PathFigure::StartPointProperty = DependencyObject::Register (Value::PATHFIGURE, "StartPoint", Value::POINT);

	/* ArcSegment fields */
	ArcSegment::IsLargeArcProperty = DependencyObject::Register (Value::ARCSEGMENT, "IsLargeArc", new Value (false));
	ArcSegment::PointProperty = DependencyObject::Register (Value::ARCSEGMENT, "Point", Value::POINT);
	ArcSegment::RotationAngleProperty  = DependencyObject::Register (Value::ARCSEGMENT, "RadiusY", new Value (0.0));
	ArcSegment::SizeProperty = DependencyObject::Register (Value::ARCSEGMENT, "RotationAngle", Value::POINT);
	ArcSegment::SweepDirectionProperty = DependencyObject::Register (Value::ARCSEGMENT, "SweepDirection", new Value (SweepDirectionCounterclockwise));

	/* BezierSegment fields */
	BezierSegment::Point1Property = DependencyObject::Register (Value::BEZIERSEGMENT, "Point1", Value::POINT);
	BezierSegment::Point2Property = DependencyObject::Register (Value::BEZIERSEGMENT, "Point2", Value::POINT);
	BezierSegment::Point3Property = DependencyObject::Register (Value::BEZIERSEGMENT, "Point3", Value::POINT);

	/* LineSegment fields */
	LineSegment::PointProperty = DependencyObject::Register (Value::LINESEGMENT, "Point", Value::POINT);

	/* PolyBezierSegment fields */
	PolyBezierSegment::PointsProperty = DependencyObject::Register (Value::POLYBEZIERSEGMENT, "Point", Value::POINT);

	/* PolyLineSegment fields */
	PolyLineSegment::PointsProperty = DependencyObject::Register (Value::POLYLINESEGMENT, "Point", Value::POINT);

	/* PolyQuadraticBezierSegment field */
	PolyQuadraticBezierSegment::PointsProperty = DependencyObject::Register (Value::POLYQUADRATICBEZIERSEGMENT, "Point", Value::POINT);

	/* QuadraticBezierSegment field */
	QuadraticBezierSegment::Point1Property = DependencyObject::Register (Value::QUADRATICBEZIERSEGMENT, "Point1", Value::POINT);
	QuadraticBezierSegment::Point2Property = DependencyObject::Register (Value::QUADRATICBEZIERSEGMENT, "Point2", Value::POINT);
}
