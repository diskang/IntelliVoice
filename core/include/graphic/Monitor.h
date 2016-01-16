// Monitor.h
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

#ifndef SSI_GRAPHIC_MONITOR_H
#define SSI_GRAPHIC_MONITOR_H

#include "base/IMonitor.h"

namespace ssi {

class Monitor : public IMonitor {

public:

	Monitor (ssi_size_t maxlen = SSI_MAX_CHAR);
	virtual ~Monitor ();
	
	void print (const ssi_char_t *str);	
	void update();
	void setPosition(ssi_rect_t rect);
	void clear ();

	void setFont(const ssi_char_t *name, ssi_size_t size);

protected:
	
	void createEdit(ssi_handle_t hwnd);
	#if _WIN32||_WIN64
	LRESULT CALLBACK windowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	#else
	int WindowProc(int hwnd, int msg, int wParam, int lParam);
	#endif
	ssi_handle_t _hWnd;
	ssi_handle_t _hEdit;
	ssi_handle_t _hFont;

	int _scroll_v, _scroll_h;

	ssi_char_t *_buffer;
	ssi_size_t _n_buffer;
	ssi_size_t _buffer_count;
};

}

#endif
