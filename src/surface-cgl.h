/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * surface-glx.h
 *
 * Copyright 2010 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 *
 */

#ifndef __MOON_SURFACE_CGL_H__
#define __MOON_SURFACE_CGL_H__

#include "surface-gl.h"

#include <OpenGL/OpenGL.h>

#ifdef __MOON_CGL__

namespace Moonlight {

class MOON_API CGLSurface : public GLSurface {
public:
	CGLSurface (CGLContextObj context, GLsizei w, GLsizei h);
	CGLSurface (GLsizei w, GLsizei h);

	CGLContextObj GetContext () { return context; }

	void SwapBuffers ();
	void Reshape (int width, int height);
	cairo_surface_t *Cairo ();
	GLuint Texture ();
	GLuint TextureYUV (int i);
	bool HasTexture ();

private:
	CGLContextObj context;
};

};

#endif /* __MOON_CGL__ */

#endif /* __MOON_SURFACE_CGL_H__ */