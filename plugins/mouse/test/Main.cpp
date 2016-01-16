// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/02/28
// Copyright (C) 2007-14 University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#include "ssi.h"
using namespace ssi;

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

bool ex_mouse (void *arg);

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);
	Factory::RegisterDLL ("ssimouse");
	Factory::RegisterDLL ("ssiframe");
	Factory::RegisterDLL ("ssievent");
	
	Factory::RegisterDLL ("ssiioput");
	#if _WIN32||_WIN64
	Factory::RegisterDLL ("ssigraphic");
	#endif
	

	Exsemble ex;
	ex.add(&ex_mouse, 0, "MOUSE", "How to use 'Mouse' sensor in a pipeline.");	
	ex.show();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

bool ex_mouse (void *arg) {
#if _WIN32||_WIN64
	ITheFramework *frame = Factory::GetFramework ();
	ssi_pcast (TheFramework, frame)->getOptions ()->setConsolePos (0, 400, 600, 400);
	ITheEventBoard *board = Factory::GetEventBoard ();
	IThePainter *painter = Factory::GetPainter ();
	
	Mouse *mouse = ssi_create (Mouse, "mouse", true);
	mouse->getOptions ()->mask = Mouse::LEFT;
	mouse->getOptions ()->flip = true;
	mouse->getOptions ()->scale = true;
	mouse->getOptions()->single = false;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider (mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);
	frame->AddSensor (mouse);
	
	FileWriter *cursor_write = ssi_create (FileWriter, 0, true);
	cursor_write->getOptions ()->setPath ("cursor");
	cursor_write->getOptions ()->stream = true;
	cursor_write->getOptions ()->type = File::ASCII;
	frame->AddConsumer (cursor_p, cursor_write, "0.25s");

	FileWriter *button_write = ssi_create (FileWriter, 0, true);
	button_write->getOptions ()->setPath ("button");
	button_write->getOptions ()->stream = true;
	button_write->getOptions ()->type = File::ASCII;
	frame->AddConsumer (button_p, button_write, "0.25s");

	ZeroEventSender *ezero = ssi_create (ZeroEventSender, "ezero", true);
	ezero->getOptions ()->mindur = 0.2;	
	frame->AddConsumer (button_p, ezero, "0.25s");
	board->RegisterSender (*ezero);

	SignalPainter *mouse_plot = ssi_create (SignalPainter, 0, true);
	mouse_plot->getOptions ()->setName ("mouse");
	mouse_plot->getOptions ()->size = 0;
	mouse_plot->getOptions ()->fix[0] = 0;
	mouse_plot->getOptions ()->fix[1] = 1;
	mouse_plot->getOptions ()->autoscale = false;
	mouse_plot->getOptions ()->type = PaintSignalType::PATH;
	frame->AddConsumer (cursor_p, mouse_plot, "1", "20"); 

	SignalPainter *mouse_tr_plot = ssi_create (SignalPainter, 0, true);
	mouse_tr_plot->getOptions ()->setName ("mouse (tr)");
	mouse_tr_plot->getOptions ()->setArrange (1, 2, 0, 0, 600, 400);
	frame->AddEventConsumer (cursor_p, mouse_tr_plot, board, ezero->getEventAddress ()); 

	frame->Start ();
	board->Start ();
	frame->Wait ();
	board->Stop ();
	frame->Stop ();
	board->Clear ();
	frame->Clear ();
	painter->Clear ();
#else
	ITheFramework *frame = Factory::GetFramework ();

	Mouse *mouse = ssi_create (Mouse, "mouse", true);
    mouse->connect();
	
	mouse->getOptions ()->mask = Mouse::LEFT;
	
	mouse->getOptions ()->flip = true;
	mouse->getOptions ()->scale = true;
	mouse->getOptions()->single = false;
	
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider (mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);
	frame->AddSensor (mouse);

    FileWriter *cursor_write = ssi_create (FileWriter, 0, true);
    cursor_write->getOptions ()->setPath ("cursor");
    cursor_write->getOptions ()->stream = true;
    cursor_write->getOptions ()->type = File::ASCII;
    frame->AddConsumer (cursor_p, cursor_write, "0.25s");

    FileWriter *button_write = ssi_create (FileWriter, 0, true);
    button_write->getOptions ()->setPath ("button");
    button_write->getOptions ()->stream = true;
    button_write->getOptions ()->type = File::ASCII;
    frame->AddConsumer (button_p, button_write, "0.25s");

    ZeroEventSender *ezero = ssi_create (ZeroEventSender, "ezero", true);
    ezero->getOptions ()->mindur = 0.2;
    frame->AddConsumer (button_p, ezero, "0.25s");


    frame->Start ();

    frame->Wait ();

    frame->Stop ();

    frame->Clear ();

#endif
	return true;
}
