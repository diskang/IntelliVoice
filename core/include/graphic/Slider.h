// Slider.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/10/15 
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

#ifndef SSI_GRAPHIC_SLIDER_H
#define SSI_GRAPHIC_SLIDER_H

#include "base/IWindow.h"

namespace ssi {

class Slider : public IWindowClient {

public:
	
	class ICallback {

	public:
	
		virtual void update (ssi_real_t value) = 0;
	};

public:

	Slider (const ssi_char_t *name,
		ssi_real_t value,
		ssi_real_t min,
		ssi_real_t max,
		ssi_size_t steps = 100);
	~Slider ();

	void update();
	void setPosition(ssi_rect_t rect);
	
	void set (ssi_real_t value);
	ssi_real_t get ();

	void setCallback (ICallback *callback) {
		_callback = callback;
	};

protected:

	void createSlider(ssi_handle_t hwnd);
#if _WIN32||_WIN64
	LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#else
	int WindowProc(int hwnd, int msg, int wParam, int lParam);
#endif
	ssi_handle_t _hWnd;
	ssi_handle_t _hSlider;

	ssi_size_t _steps;
	ssi_real_t _value, _min, _max;
	ssi_char_t *_name;

	ICallback *_callback;
};

}

#endif
