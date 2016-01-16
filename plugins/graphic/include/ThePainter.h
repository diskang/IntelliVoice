// ThePainter.h
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

/* Offers some basic plotting functionality. */

#ifndef SSI_GRAPHIC_THEPAINTER_H
#define SSI_GRAPHIC_THEPAINTER_H

#include "base/IObject.h"
#include "base/IThePainter.h"
#include "ioput/option/OptionList.h"
#include "thread/Thread.h"
#include "thread/Lock.h"

namespace ssi {

// Maximum number of canvas objects handled by the painter
#define SSI_THEPAINTER_MAX_WINDOWS 64

class ThePainter : public IThePainter {

friend class Factory;

public: 

	class Options : public OptionList {

	public:
		Options () 
			: arrange (false), console (false), relative (false) {

			screen[0] = 0;
			screen[1] = 0;

			apos[0] = 1;
			apos[1] = 1;
			apos[2] = 0;
			apos[3] = 0;
			apos[4] = 400;
			apos[5] = 600;

			cpos[0] = 400;
			cpos[1] = 0;
			cpos[2] = 400;
			cpos[3] = 600;

			addOption ("relative", &relative, 1, SSI_BOOL, "arrange windows relative to screen");	
			addOption ("screen", &screen, 2, SSI_REAL, "customize screen region [width,height], by default set to desktop size");
			addOption ("arrange", &arrange, 1, SSI_BOOL, "arrange windows");				
			addOption ("apos", &apos, 6, SSI_REAL, "position of arranged windows [numh,numv,posx,posy,width,height], either in pixels or relative to screen");
			addOption ("console", &console, 1, SSI_BOOL, "move console window");	
			addOption ("cpos", &cpos, 4, SSI_REAL, "position of console window on screen [posx,posy,width,height], either in pixels or relative to screen");
		}

		void SetScreen (ssi_real_t width, ssi_real_t height) {
			screen[0] = width;
			screen[1] = height;
		}

		void Arrange (ssi_real_t numh, ssi_real_t numv, ssi_real_t x, ssi_real_t y, ssi_real_t width, ssi_real_t height) {
			arrange = true;
			apos[0] = numh;
			apos[1] = numv;
			apos[2] = x;
			apos[3] = y;
			apos[4] = width;
			apos[5] = height;
		}

		void Console (ssi_real_t x, ssi_real_t y, ssi_real_t width, ssi_real_t height) {
			console = true;
			cpos[0] = x;
			cpos[1] = y;
			cpos[2] = width;
			cpos[3] = height;
		}

		bool arrange;
		bool relative;
		ssi_real_t apos[6];		
		bool console;
		ssi_real_t cpos[4];
		ssi_real_t screen[2];
	};

public:

	Options *getOptions () { return &_options; }
	static const ssi_char_t *GetCreateName () { return "ssi_object_ThePainter"; }
	const ssi_char_t *getName () { return GetCreateName(); }
	const ssi_char_t *getInfo () { return "Creates windows and handles paint operations."; }
	static IObject* Create(const char* file) { return new ThePainter (file); }

    void Clear ();
	int AddCanvas(const char* name);
    bool RemCanvas (int canvas_id);
	bool AddObject(int canvas_id, ICanvasClient *object);
	bool Clear(int canvas_id);
	bool Update (int canvas_id);
	bool Move(int canvas_id, int x, int y, int width, int height, bool paramsDescribeClientRect = false);

	//   __________________________________________
	//  |(0,0)                        numh=3       |
	//  |     (start_x,start_y)       numv=2       |
	//  |      \/_________width___________         |
    //  |      |        |        |        |        |
	//  |      |        |        |        |        |
	//  |      |________|________|________|height  |
	//  |      |        |        |        |        |
	//  |      |        |        |        |        |
	//  |      |________|________|________|        |
	//  |                                          |
	//  |__________________________________________|
    //
	void Arrange (int numh, int numv, int start_x, int start_y, int width, int height);
	void ArrangeRelative (int numh, int numv, ssi_real_t start_x, ssi_real_t start_y, ssi_real_t width, ssi_real_t height);
	void MoveConsole (int x, int y, int width, int height);
	void MoveConsoleRelative (ssi_real_t x, ssi_real_t y, ssi_real_t width, ssi_real_t height);
	void SetScreen (ssi_real_t width, ssi_real_t height);

private:

    ThePainter (const ssi_char_t *file);
    ~ThePainter ();
	ssi_char_t *_file;
	Options _options;

	IWindow *_window[SSI_THEPAINTER_MAX_WINDOWS];
	ICanvas *_canvas[SSI_THEPAINTER_MAX_WINDOWS];
	Mutex _mutex[SSI_THEPAINTER_MAX_WINDOWS];

	ULONG_PTR _gdiplus;
};

}

#endif // _THEPAINTER_H
