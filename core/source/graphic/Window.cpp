// Window.cpp
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

#include "graphic/Window.h"
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

const char *Window::_wClassName = "SSIWindow";
ssi_handle_t Window::_hParent = 0;
ssi_handle_t Window::_hInstance = 0;

Window::Window ()
: Thread (true),
	_client (0),
	_style(STYLES::DEFAULT),
	_hWnd(0)  {

	RegisterWindowClass();

	_position.left = 0;
	_position.top = 0;
	_position.height = 100;
	_position.width = 100;

	_title[0] = '\0';
}

Window::~Window () {

	if (_hWnd) {
		::DestroyWindow((HWND) _hWnd);
	}
}

void Window::setClient(IWindowClient *client) {
	_client = client;
}

void Window::PrintLastError()
{
	LPTSTR lpMsgBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	ssi_wrn(lpMsgBuf);
}

void Window::RegisterWindowClass() {

	_hParent = GetConsoleWindow();
	_hInstance = ::GetModuleHandle(NULL);

	WNDCLASS wndcls;
	BOOL result = ::GetClassInfo((HINSTANCE) _hInstance, _wClassName, &wndcls);

	if (result == 0) {

		WNDCLASSEX wClass;
		wClass.cbSize = sizeof(WNDCLASSEX);
		wClass.style = 0;
		wClass.lpfnWndProc = WindowProc;
		wClass.cbClsExtra = 0;
		wClass.cbWndExtra = 0;
		wClass.hInstance = (HINSTANCE) _hInstance;
		wClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wClass.lpszMenuName = NULL;
		wClass.lpszClassName = _wClassName;
		wClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&wClass))
		{
			PrintLastError();
			ssi_err("window registration failed");
		}
	}
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE) {
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lParam;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (long)cs->lpCreateParams);
	}

	Window *me = (Window *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (me && me->_client) {		
		return me->windowProc(hWnd, msg, wParam, lParam);
	} else {
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
}


LRESULT CALLBACK Window::windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {			   

	case WM_WINDOWPOSCHANGED:

		if (_client) {
			RECT rect;
			::GetClientRect((HWND)_hWnd, &rect);
			ssi_rect_t r;
			r.left = rect.left;
			r.top = rect.top;
			r.width = rect.right - rect.left;
			r.height = rect.bottom - rect.top;
			_client->setPosition(r);
		}
		return 0;

	case WM_CLOSE:

		::DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:

		::PostQuitMessage(0);
		return 0;

	//case WM_ERASEBKGND:

	//	return 1;
	}

	if (_client) {
		return _client->windowProc(hWnd, msg, wParam, lParam);
	} else {
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

void Window::enter() {
	
	_hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		_wClassName,
		_title,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		0, 0, 0, 0,
		(HWND)_hParent,
		0,
		(HINSTANCE)_hInstance,
		this);

	if (!_hWnd)
	{
		PrintLastError();
		ssi_wrn("could not create window");
	} else {
		setStyle(_style);
		setTitle(_title);
		setPosition(_position);
	}
}

void Window::run () {

	MSG msg;
	while (::GetMessage(&msg, NULL, NULL, NULL) > 0) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);	
	}
}

void Window::create() {

	start();
}

void Window::show() {

	if (_hWnd) {
		::ShowWindow((HWND)_hWnd, SW_SHOW);
		::UpdateWindow((HWND)_hWnd);
	}
}

void Window::update() {

	if (_client) {
		_client->update();
	}	
}

void Window::setTitle(const ssi_char_t *title) {

	Thread::setName(title);
	ssi_strcpy(_title, title);

	if (_hWnd) {
		::SendMessage((HWND)_hWnd, WM_SETTEXT, ssi_strlen(title) + 1, (LPARAM)title);
	}
}

void Window::setPosition(ssi_rect_t rect, bool paramsDescribeClientRect) {

	if (paramsDescribeClientRect) {
		RECT wndwRC, clntRC;
		::GetWindowRect((HWND) _hWnd, &wndwRC);
		::GetClientRect((HWND)_hWnd, &clntRC);
		long dx = (wndwRC.right - wndwRC.left) - (clntRC.right - clntRC.left);
		long dy = (wndwRC.bottom - wndwRC.top) - (clntRC.bottom - clntRC.top);
		rect.width = rect.width + dx;
		rect.height = rect.height + dy;
	}

	_position = rect;

	if (_hWnd) {
		::SetWindowPos((HWND)_hWnd, HWND_TOP, rect.left, rect.top, rect.width, rect.height, 0);
	}
}

void Window::setStyle(STYLE style) {

	_style = style;

	if (_hWnd) {
		if (_style & STYLES::NO_CLOSE) {
			::EnableMenuItem(::GetSystemMenu((HWND)_hWnd, false), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
		}
		if (_style & STYLES::NO_MINIMIZE) {
			::SetWindowLong((HWND)_hWnd, GWL_STYLE, ::GetWindowLong((HWND)_hWnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
		}
		if (_style & STYLES::NO_MAXIMIZE) {
			::SetWindowLong((HWND)_hWnd, GWL_STYLE, ::GetWindowLong((HWND)_hWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
		}
	}
}

void Window::close() {

	if (_hWnd) {
		::SendMessage((HWND)_hWnd, WM_CLOSE, 0, 0);
	}
}





}
