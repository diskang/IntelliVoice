// ThePainter.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/07
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

#include "ThePainter.h"
#include "GraphicLibCons.h"
#if HAS_CAIRO
#include "graphic/SDL_Canvas.h"
#include "graphic/SDL_Window.h"
#else
#include "graphic/Canvas.h"
#include "graphic/Window.h"
#endif
#include "graphic/Colormap.h"

using namespace std;
#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif


namespace ssi {

ssi_char_t *ssi_log_name = "painter___";

// constructor
ThePainter::ThePainter (const ssi_char_t *file)
: _file (0) {

#ifdef _THEPAINTER_DEBUG
	cout << "Initialize painter.. ";
#endif
#if _WIN32||_WIN64
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	if(Gdiplus::GdiplusStartup(&_gdiplus, &gdiplusStartupInput, NULL) != Gdiplus::Ok)
	{
		ssi_err ("failed call to GdiplusStartup() in ThePainter constructor");
	}
#endif
    // an array with pointer to all canvas handled by the painter
	for (int i = 0; i < SSI_THEPAINTER_MAX_WINDOWS; i++) {
		_canvas[i] = 0;
		_window[i] = 0;
	}

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}

#ifdef _THEPAINTER_DEBUG
	cout << "ok" << endl;
#endif
}

// deconstructor
ThePainter::~ThePainter () {

	Clear();	
#if _WIN32||_WIN64
	Gdiplus::GdiplusShutdown (_gdiplus);
#endif
	_gdiplus = NULL;

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

// shutdown function
void ThePainter::Clear () {

#ifdef _THEPAINTER_DEBUG
printf ("Shutting down painter ..\n");
#endif

    // destroy all active canvas
	for (int i = 0; i < SSI_THEPAINTER_MAX_WINDOWS; i++) {
        if (!(_canvas[i])) continue;
		RemCanvas (i);
    }

#ifdef _THEPAINTER_DEBUG
printf ("Good bye!\n");
#endif
}

// add a new canvas
int ThePainter::AddCanvas(const char* name) {

#ifdef _THEPAINTER_DEBUG
	cout << "Add new _canvas..";
#endif

   	// find a free canvas id
	int canvas_id = -1;
	for (int i = 0; i < SSI_THEPAINTER_MAX_WINDOWS; i++) {
		// get _mutex for the buffer
		Lock lock (_mutex[i]);
		// check if _canvas is still available
		if (!_canvas[i]) {
			canvas_id = i;
			break;
		}
	}
	if (canvas_id == -1) {
#ifdef _THEPAINTER_DEBUG
{
	Lock lock (logmutex);
	cout << "error [all _canvas in use]";
}
#endif
		return -1;
	}

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// create canvas	
	_canvas[canvas_id] = new Canvas();
	// start window
	_window[canvas_id] = new Window();
	_window[canvas_id]->setClient(_canvas[canvas_id]);
	_window[canvas_id]->setTitle(name);
	_window[canvas_id]->setStyle(IWindow::STYLES::NO_CLOSE);
	_window[canvas_id]->create ();
	_window[canvas_id]->show();
	// arrange
	if (_options.arrange) {
		if (_options.relative) {
			ArrangeRelative (ssi_cast (int, _options.apos[0]), ssi_cast (int, _options.apos[1]), _options.apos[2], _options.apos[3], _options.apos[4], _options.apos[5]);
		} else {
			Arrange (ssi_cast (int, _options.apos[0]), ssi_cast (int, _options.apos[1]), ssi_cast (int, _options.apos[2]), ssi_cast (int, _options.apos[3]), ssi_cast (int, _options.apos[4]), ssi_cast (int, _options.apos[5]));
		}
	}
	// console
	if (_options.console) {
		if (_options.relative) {
			MoveConsoleRelative (_options.cpos[0], _options.cpos[1], _options.cpos[2], _options.cpos[3]);
		} else {
			MoveConsole (ssi_cast (int, _options.cpos[0]), ssi_cast (int, _options.cpos[1]), ssi_cast (int, _options.cpos[2]), ssi_cast (int, _options.cpos[3]));
		}
	}
#ifdef _THEPAINTER_DEBUG
cout << "ok" << endl;
#endif

    return canvas_id;
}

// removes a canvas
bool ThePainter::RemCanvas (int canvas_id) {

#ifdef _THEPAINTER_DEBUG
cout << "Remove _canvas with ID " << canvas_id << ".. ";
#endif

    // first check if ID is valid
    if (canvas_id < 0 || canvas_id >= SSI_THEPAINTER_MAX_WINDOWS) {
#ifdef _THEPAINTER_DEBUG
cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// now check if canvas is in use
    if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// close window and free memory
	_window[canvas_id]->close();	
	delete _window[canvas_id];
	_window[canvas_id] = 0;
	delete _canvas[canvas_id];
	_canvas[canvas_id] = 0;

#ifdef _THEPAINTER_DEBUG
cout <<  "ok" << endl;
#endif

    return true;
}

// add object to canvas
bool ThePainter::AddObject(int canvas_id, ICanvasClient *object) {

#ifdef _THEPAINTER_DEBUG
cout << "Plot on _canvas with ID " << canvas_id << ".. ";
#endif

    // first check if ID is valid
    if (canvas_id < 0 || canvas_id >= SSI_THEPAINTER_MAX_WINDOWS) {
#ifdef _THEPAINTER_DEBUG
cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// now check if canvas is in use
    if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// plot and update
	_canvas[canvas_id]->addClient (object);

#ifdef _THEPAINTER_DEBUG
cout <<  "ok" << endl;
#endif

    return true;
}

// updates a canvas
bool ThePainter::Update(int canvas_id) {

#ifdef _THEPAINTER_DEBUG
cout << "Plot on _canvas with ID " << canvas_id << ".. ";
#endif

    // first check if ID is valid
    if (canvas_id < 0 || canvas_id >= SSI_THEPAINTER_MAX_WINDOWS) {
#ifdef _THEPAINTER_DEBUG
cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// now check if canvas is in use
    if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// plot and update
	_canvas[canvas_id]->update ();

#ifdef _THEPAINTER_DEBUG
cout <<  "ok" << endl;
#endif

    return true;
}

// clear a canvas
bool ThePainter::Clear(int canvas_id) {

#ifdef _THEPAINTER_DEBUG
	cout << "Plot on _canvas with ID " << canvas_id << ".. ";
#endif

	// first check if ID is valid
	if (canvas_id < 0 || canvas_id >= SSI_THEPAINTER_MAX_WINDOWS) {
#ifdef _THEPAINTER_DEBUG
		cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
		return false;
	}

	// now check if canvas is in use
	if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
		cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
		return false;
	}

	// lock mutex for the canvas
	Lock lock(_mutex[canvas_id]);
	// plot and update
	_canvas[canvas_id]->clear();

#ifdef _THEPAINTER_DEBUG
	cout << "ok" << endl;
#endif

	return true;
}

// resize a canvas
bool ThePainter::Move (int canvas_id, int x, int y, int width, int height, bool paramsDescribeClientRect) {

#ifdef _THEPAINTER_DEBUG
cout << "Plot on _canvas with ID " << canvas_id << ".. ";
#endif

    // first check if ID is valid
    if (canvas_id < 0 || canvas_id >= SSI_THEPAINTER_MAX_WINDOWS) {
#ifdef _THEPAINTER_DEBUG
cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// now check if canvas is in use
    if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// plot and update
	ssi_rect_t rect;
	rect.left = x;
	rect.top = y;
	rect.width = width;
	rect.height = height;
	_window[canvas_id]->setPosition (rect, paramsDescribeClientRect);

#ifdef _THEPAINTER_DEBUG
cout <<  "ok" << endl;
#endif

    return true;
}

void ThePainter::Arrange (int numh, int numv, int start_x, int start_y, int width, int height) {
		
	int next_x = start_x;
	int next_y = start_y;
	int numh_left = numh;
	int max_steps = numh * numv;
	int win_width = width / numh;
	int win_height = height / numv;

	for (int i = 0; i < SSI_THEPAINTER_MAX_WINDOWS; i++) {

		if (max_steps <= 0) {
			break;
		}

		if (_canvas[i]) {
			// lock mutex for the canvas
			Lock lock (_mutex[i]);
			// move canvas
			ssi_rect_t rect;
			rect.left = next_x;
			rect.top = next_y;
			rect.width = win_width;
			rect.height = win_height;
			_window[i]->setPosition (rect);
			numh_left--;
			max_steps--;
			if (numh_left > 0) {
				next_x += win_width;
			} else {
				next_x = start_x;
				next_y += win_height;
				numh_left = numh;
			}
		}
	}
}

void ThePainter::ArrangeRelative (int numh, int numv, ssi_real_t start_x, ssi_real_t start_y, ssi_real_t width, ssi_real_t height) {
#if _WIN32||_WIN64
	ssi_real_t max_x, max_y;

	if (_options.screen[0] <= 0 && _options.screen[1] <= 0) {
		HWND desktop = GetDesktopWindow ();
		RECT rect;
		GetWindowRect (desktop, &rect);
		max_x = ssi_cast (ssi_real_t, rect.right);
		max_y = ssi_cast (ssi_real_t, rect.bottom);
	} else {
		max_x = _options.screen[0];
		max_y = _options.screen[1];
	}

	int next_x = ssi_cast (int, start_x * max_x);
	int next_y = ssi_cast (int, start_y * max_y);
	int numh_left = numh;
	int max_steps = numh * numv;
	int win_width = ssi_cast (int, (width * max_x) / numh);
	int win_height = ssi_cast (int, (height * max_y) / numv);

	for (int i = 0; i < SSI_THEPAINTER_MAX_WINDOWS; i++) {

		if (max_steps <= 0) {
			break;
		}

		if (_canvas[i]) {
			// lock mutex for the canvas
			Lock lock (_mutex[i]);
			// move canvas
			ssi_rect_t rect;
			rect.top = next_y;
			rect.left = next_x;
			rect.width = win_width;
			rect.height = win_height;
			_window[i]->setPosition (rect);
			numh_left--;
			max_steps--;
			if (numh_left > 0) {
				next_x += win_width;
			} else {
				next_x = ssi_cast (int, start_x * max_x);
				next_y += win_height;
				numh_left = numh;
			}
		}
	}
#endif
}

void ThePainter::MoveConsole (int start_x, int start_y, int width, int height) {
#if _WIN32||_WIN64
	HWND hWnd = GetConsoleWindow();
	::MoveWindow(hWnd, start_x, start_y, width, height, true);
#endif
}

void ThePainter::MoveConsoleRelative (ssi_real_t start_x, ssi_real_t start_y, ssi_real_t width, ssi_real_t height) {
#if _WIN32||_WIN64
	ssi_real_t max_x, max_y;

	if (_options.screen[0] <= 0 && _options.screen[1] <= 0) {
		HWND desktop = GetDesktopWindow ();
		RECT rect;
		GetWindowRect (desktop, &rect);
		max_x = ssi_cast (ssi_real_t, rect.right);
		max_y = ssi_cast (ssi_real_t, rect.bottom);
	} else {
		max_x = _options.screen[0];
		max_y = _options.screen[1];
	}

	HWND hWnd = GetConsoleWindow();
	::MoveWindow(hWnd, ssi_cast (int, start_x * max_x), ssi_cast (int, start_y * max_y), ssi_cast (int, width * max_x), ssi_cast (int, height * max_y), true);
#endif
}

void ThePainter::SetScreen (ssi_real_t width, ssi_real_t height) {
	_options.screen[0] = width;
	_options.screen[1] = height;
}

}
