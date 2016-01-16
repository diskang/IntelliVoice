// Window.h
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

#ifndef SSI_GRAPHIC_WINDOW_H
#define SSI_GRAPHIC_WINDOW_H

#include "base/IWindow.h"
#include "thread/Thread.h"

namespace ssi {

class Window : public IWindow, public Thread {

public:

	Window ();
	virtual ~Window ();

	void setClient(IWindowClient *client);
	void setTitle(const ssi_char_t *str);
	void setPosition(ssi_rect_t position, bool paramsDescribeClientRect = false);	
	void setStyle(STYLE style);

	void create();
	void show();	
	void update();
	void close();

	ssi_handle_t getHandle();

protected:

	void enter();
	void run();

	static void PrintLastError();
	static void RegisterWindowClass();
	#if _WIN32||_WIN64
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	#else
	static int WindowProc(int hwnd, int msg, int wParam, int lParam);
	int windowProc(int hwnd, int msg, int wParam, int lParam);
	#endif

	static const char *_wClassName;
	static ssi_handle_t _hParent;
	static ssi_handle_t _hInstance;
	ssi_handle_t _hWnd;
	IWindowClient *_client;

	ssi_char_t _title[SSI_MAX_CHAR];
	ssi_rect_t _position;
	STYLE _style;
};

}




#endif
