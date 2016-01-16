// ICanvas.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/05/28
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

#ifndef SSI_ICANVAS_H
#define SSI_ICANVAS_H

#include "base/IColormap.h"
#include "base/IWindow.h"


namespace ssi {

class ICanvas;

class ICanvasClient {

public:

	virtual ~ICanvasClient() {};

	virtual void paint(ICanvas *canvas, ssi_handle_t context, ssi_rect_t area) = 0;
	virtual void clear() {};
	virtual void mouseDown(ICanvas *canvas, ssi_point_t position, IWindowClient::KEY vkey) {};
	virtual void keyDown(ICanvas *canvas, IWindowClient::KEY key, IWindowClient::KEY vkey) {};
};

class ICanvas : public IWindowClient {

public:

	virtual ~ICanvas() {};
	
	virtual void addClient(ICanvasClient *client) = 0;
	virtual void clear() {};	
};

}

#endif
