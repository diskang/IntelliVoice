// Canvas.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/13
// Copyright (C) 2007-14 University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#pragma once

#ifndef SSI_GRAPHIC_CANVAS_H
#define SSI_GRAPHIC_CANVAS_H

#include "base/ICanvas.h"
#include "graphic/Window.h"
#include "thread/Lock.h"

namespace ssi {

class Canvas : public ICanvas {

public:

	Canvas();
	~Canvas ();

	void update();
	void setPosition(ssi_rect_t rect);
	
	void addClient(ICanvasClient *client);
	void setColormap(IColormap::COLORMAP::List type);

protected:

	void paint();	
	void paint_objects();

	void mouseDown(ssi_point_t position, KEY vkey);
	void keyDown(KEY key, KEY vkey);

	#if _WIN32||_WIN64
	LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	#else
	int windowProc(int hwnd, int msg, int wParam, int lParam);
	#endif

	ssi_handle_t _hwnd;
	Mutex _mutex;
	std::vector<ICanvasClient *> _objects;

	ssi_handle_t _context;
	ssi_handle_t _context_mem;
	ssi_handle_t _bitmap;
	int _width, _height;	
	ssi_rect_t _rect;
};

}

#endif
