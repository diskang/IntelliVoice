// Monitor.cpp
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

#include "graphic/Monitor.h"
#include "base/Factory.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif


namespace ssi {

Monitor::Monitor (ssi_size_t maxchar)
	: _hWnd (0),
	_hEdit (0),
	_hFont (0) {

	_n_buffer = maxchar + 1;
	_buffer = new ssi_char_t[_n_buffer];

	clear();

	_hFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FIXED_PITCH, TEXT("Courier New"));
}

Monitor::~Monitor () {
	
	delete[] _buffer; _buffer = 0;
	if (_hFont) {
		::DeleteObject(_hFont);
	}
	if (_hEdit) {
		::DestroyWindow((HWND)_hWnd);
	}

}

void Monitor::setFont(const ssi_char_t *name, ssi_size_t size) {

	ssi_handle_t hFont = CreateFont(size, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FIXED_PITCH, TEXT(name));
	if (!hFont) {
		ssi_wrn("could not create font '%s'", name)
	} else {
		::DeleteObject(_hFont);
		_hFont = hFont;
	}

	if (_hEdit) {
		::SendMessage((HWND)_hEdit, WM_SETFONT, (WPARAM)_hFont, MAKELPARAM(TRUE, 0));
	}
}

LRESULT CALLBACK Monitor::windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (!_hWnd) {
		_hWnd = hWnd;
	}

	switch (msg) {
	
	case WM_CREATE: {
		createEdit(hWnd);
		return 0;
	}

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);;;
}

void Monitor::createEdit(ssi_handle_t hWnd) {

	_hEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
		"EDIT",
		"",
		WS_CHILD | WS_VISIBLE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL |
		ES_MULTILINE | WS_VSCROLL | WS_HSCROLL,
		0,
		0,
		0,
		0,
		(HWND) hWnd,
		0,
		::GetModuleHandle(NULL),
		0);

	if (!_hEdit) {
		//PrintLastError();
		ssi_wrn("could not create edit");
	} else {		
		::SendMessage((HWND)_hEdit, WM_SETFONT, (WPARAM)_hFont, MAKELPARAM(TRUE, 0));
		update();
	}
}

void Monitor::clear() {

	_buffer[0] = '\0';
	_buffer_count = 1;
}

void Monitor::print(const ssi_char_t *str) {

	ssi_size_t len = ssi_cast(ssi_size_t, strlen(str));
	if (_buffer_count + len >= _n_buffer - 1) {
		//ssi_wrn ("max buffer size reached '%u', crop string", _n_buffer);
		len = _n_buffer - _buffer_count - 1;
	}
	if (len > 0) {
		memcpy(_buffer + _buffer_count - 1, str, len);
		_buffer_count += len;
		_buffer[_buffer_count - 1] = '\0';
	}
}

void Monitor::setPosition(ssi_rect_t rect) {

	::MoveWindow((HWND)_hEdit, 0, 0, rect.width, rect.height, TRUE);

	update();
}


void Monitor::update() {

	if (_hEdit) {

		::SendMessage((HWND)_hEdit, WM_SETREDRAW, FALSE, 0);
		::SendMessage((HWND)_hEdit, WM_SETTEXT, _buffer_count, (LPARAM)_buffer);
		::SendMessage((HWND)_hEdit, EM_SCROLL, _scroll_v, 0);
		::SendMessage((HWND)_hEdit, WM_VSCROLL, _scroll_v, NULL);
		::SendMessage((HWND)_hEdit, WM_HSCROLL, _scroll_h, NULL);
		::SendMessage((HWND)_hEdit, WM_SETREDRAW, TRUE, 0);

		RECT rect;
		::GetClientRect((HWND)_hWnd, &rect);
		::MoveWindow((HWND)_hEdit, 0, 0, rect.right - rect.left, rect.bottom - rect.top, TRUE);

		InvalidateRect((HWND)_hEdit, NULL, FALSE);
		UpdateWindow((HWND)_hEdit);
	}
}




}
