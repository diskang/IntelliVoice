// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/06
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

#include "PaintBackground.h"
#include "PaintRandomLines.h"
#include "PaintRandomPoints.h"
#include "PaintRandomShapes.h"
#include "PaintSomeText.h"
#include "MyCanvasClient.h"

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

bool ex_slider (void *arg);
bool ex_canvas(void *arg);
bool ex_painter(void *arg);
bool ex_stream(void *arg);
bool ex_pipeline(void *arg);
bool ex_monitor(void *arg);

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssiframe");
	Factory::RegisterDLL ("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssievent");
	Factory::RegisterDLL ("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssiioput");
	Factory::RegisterDLL ("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssimouse");
	Factory::RegisterDLL ("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssigraphic");

	ssi_random_seed();

	Exsemble ex;	
	ex.add(&ex_canvas, 0, "CANVAS", "How to use 'Canvas' to draw some random shapes.");	
	ex.add(&ex_stream, 0, "STREAMS", "How to use 'GraphicTools' to draw streams.");
	ex.add(&ex_painter, 0, "PAINTER", "How to use 'ThePainter' to draw streams.");
	ex.add(&ex_pipeline, 0, "PIPELINE", "How to use 'SignalPainter' and 'VideoPainter' to draw the streams in a pipeline.");
	ex.add(&ex_monitor, 0, "MONITOR", "How to use 'Monitor' to output text messages.");
	ex.add(&ex_slider, 0, "SLIDER", "How to use 'Slider' class.");
	ex.show();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

bool ex_slider (void *arg) {

	Slider slider ("slider", 0, -20, 20, 40);
	
	ssi::Window window;
	window.setClient(&slider);
	window.setPosition(ssi_rect(0, 0, 200, 75));
	window.setStyle(IWindow::STYLES::NO_CLOSE | IWindow::STYLES::NO_MAXIMIZE);
	window.create();
	window.show ();
	
	slider.set (0.5f);
	printf ("mew slider value: %.2f\n", slider.get ());
	
	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();

	window.close();

	return true;
}

bool ex_canvas(void *arg) {

	IThePainter *painter = Factory::GetPainter();

	PaintBackground background;
	PaintRandomPoints points(1000);
	PaintRandomLines lines(30);
	PaintRandomShapes shapes(10);
	PaintSomeText text(5, "Arial", "Hello World");

	Canvas canvas[5];
	canvas[0].addClient(&background);
	canvas[0].addClient(&points); 	
	canvas[1].addClient(&background);
	canvas[1].addClient(&lines);	
	canvas[2].addClient(&background);
	canvas[2].addClient(&shapes);	
	canvas[3].addClient(&background);
	canvas[3].addClient(&text);
	canvas[4].addClient(&background);
	canvas[4].addClient(&points);
	canvas[4].addClient(&lines);
	canvas[4].addClient(&shapes);
	canvas[4].addClient(&text);

	ssi::Window window[5];	
	ssi_char_t *titles[] = { "points", "lines", "shapes", "text", "all" };
	for (ssi_size_t i = 0; i < 5; i++) {
		window[i].setClient(&canvas[i]);
		window[i].setPosition(ssi_rect(0, i * 200, 400, 200));
		window[i].setTitle(titles[i]);
		window[i].setStyle(IWindow::STYLES::NO_CLOSE | IWindow::STYLES::NO_MINIMIZE | IWindow::STYLES::NO_MAXIMIZE);
		window[i].create();
		window[i].show();
	}

	ssi_print("\n\n\tpress enter to continue\n");
	getchar();

	for (ssi_size_t i = 0; i < 5; i++) {
		window[i].close();
	}

	return true;
}

bool ex_painter (void *arg) {	

	int id;

	IThePainter *painter = Factory::GetPainter ();

	ssi_stream_t data;
	
	FileTools::ReadStreamFile("E:/code/intelliVoice/plugins/graphic/test/eye", data);

	PaintData paint_signal;	
	paint_signal.setData (data, PaintData::TYPE::SIGNAL);
	id = painter->AddCanvas ("signal");
	painter->AddObject (id, &paint_signal);	
	painter->Update (id);	
	
	PaintData paint_path;
	paint_path.setData (data, PaintData::TYPE::PATH);
	id = painter->AddCanvas ("path");
	painter->AddObject (id, &paint_path);	
	painter->Update(id);

	PaintData paint_scatter;
	paint_scatter.setData(data, PaintData::TYPE::SCATTER);
	paint_scatter.setLabel("label");	
	id = painter->AddCanvas ("scatter");
	painter->AddObject (id, &paint_scatter);	
	painter->Update(id);

	ssi_stream_destroy(data);
	
	FileTools::ReadStreamFile("E:/code/intelliVoice/plugins/graphic/test/audio", data);
	PaintData paint_audio;
	paint_audio.setData(data, PaintData::TYPE::AUDIO);
	id = painter->AddCanvas("audio");
	painter->AddObject(id, &paint_audio);
	painter->Update(id);

	ssi_stream_destroy(data);
	
	FileTools::ReadStreamFile("E:/code/intelliVoice/plugins/graphic/test/image", data);
	PaintData imagePlot;
	imagePlot.setData(data, PaintData::TYPE::IMAGE);	
	ssi_stream_t colormap;
	FileTools::ReadStreamFile("E:/code/intelliVoice/plugins/graphic/test/colormap", colormap);
	id = painter->AddCanvas("Image");
	painter->AddObject(id, &imagePlot);
	painter->Update(id);

	ssi_stream_destroy(colormap);
	ssi_stream_destroy(data);
	
	painter->Arrange (1,5,0,0,400,800);	
	painter->MoveConsole (400,0,400,800);

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();	

	painter->Clear ();


	return true;
}

bool ex_stream (void *arg) {

	Canvas canvas[4];
	MyCanvasClient *client[4];
	ssi::Window window[4];

	{
		ssi_stream_t signal;
		ssi_stream_init(signal, 0, 3, sizeof(ssi_real_t), SSI_REAL, 100.0);
		SignalTools::Series(signal, 3.0);
		ssi_time_t freqs[] = { 1.0, 5.0, 10.0 };
		ssi_real_t amps[] = { 0.5f, 1.0f, 2.0f };
		SignalTools::Sine(signal, freqs, amps);

		ssi_norm(signal.num, signal.dim, ssi_pcast(ssi_real_t, signal.ptr));
		
		client[0] = new MyCanvasClient(signal, MyCanvasClient::TYPE::SIGNAL);

		ssi_stream_destroy(signal);
	}

	{
		ssi_stream_t signal;
		ssi_stream_init(signal, 200, 2, sizeof(ssi_real_t), SSI_REAL, 100.0);
		SignalTools::Random(signal);

		client[1] = new MyCanvasClient(signal, MyCanvasClient::TYPE::PATH);
		client[2] = new MyCanvasClient(signal, MyCanvasClient::TYPE::SCATTER);
		
		ssi_stream_destroy(signal);
	}

	{
		ssi_stream_t signal;
		ssi_stream_init(signal, 200, 200, sizeof(ssi_real_t), SSI_REAL, 100.0);
		SignalTools::Random(signal);

		client[3] = new MyCanvasClient(signal, MyCanvasClient::TYPE::IMAGE);

		ssi_stream_destroy(signal);
	}


	ssi_rect_t rect;
	rect.width = 400;
	rect.height = 200;
	rect.left = 0;
	rect.top = 0;
	for (ssi_size_t i = 0; i < 4; i++) {

		canvas[i].addClient(client[i]);
		window[i].setPosition(rect);
		window[i].setClient(&canvas[i]);
		window[i].create();
		window[i].show();

		rect.top += rect.height;
	}

	ssi_print("\n\n\tpress enter to continue\n");
	getchar();

	for (ssi_size_t i = 0; i < 4; i++) {
		window[i].close();
		delete client[i];
	}

	return true;
}

bool ex_pipeline (void *arg) {

	IThePainter *painter = Factory::GetPainter ();
	ITheFramework *frame = Factory::GetFramework ();
	ssi_pcast (TheFramework, frame)->getOptions ()->setConsolePos (400, 0, 800, 400);
	ITheEventBoard *board = Factory::GetEventBoard ();

	// sensor
	Mouse *mouse = ssi_create (Mouse, 0, true);
	mouse->getOptions ()->mask = Mouse::LEFT;	
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider (mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);	
	frame->AddSensor (mouse);

	// trigger
	ZeroEventSender *zero_trigger = ssi_create (ZeroEventSender, 0, true);	 
	frame->AddConsumer (button_p, zero_trigger, "0.2s");
	board->RegisterSender (*zero_trigger);

	// faked signal
	FakeSignal *sine = ssi_create(FakeSignal, 0, true);
	sine->getOptions()->type = FakeSignal::SIGNAL::SINE;
	ITransformable *sine_p = frame->AddProvider(sine, "fake");
	frame->AddSensor(sine);

	FakeSignal *random = ssi_create(FakeSignal, 0, true);
	random->getOptions()->type = FakeSignal::SIGNAL::RANDOM;
	ITransformable *random_p = frame->AddProvider(random, "random");
	frame->AddSensor(random);

	FakeSignal *video = ssi_create(FakeSignal, 0, true);
	video->getOptions()->type = FakeSignal::SIGNAL::IMAGE;
	ITransformable *video_p = frame->AddProvider(video, "random");
	frame->AddSensor(video);

	// plot
	SignalPainter *plot = 0;	
	
	plot = ssi_create (SignalPainter, 0, true);
	plot->getOptions ()->setName ("mouse");
	plot->getOptions ()->size = 2.0;	
	ITransformable *pp[2] = { cursor_p, button_p };
	frame->AddConsumer (2, pp, plot, "0.1s");
	
	plot = ssi_create(SignalPainter, 0, true);
	plot->getOptions()->setName("sine");
	plot->getOptions()->size = 20.0;
	frame->AddConsumer(sine_p, plot, "0.1s");
	
	plot = ssi_create(SignalPainter, 0, true);
	plot->getOptions()->setName("rand");
	plot->getOptions()->size = 2.0;
	plot->getOptions()->type = PaintSignalType::IMAGE;
	frame->AddConsumer(random_p, plot, "0.1s");

	VideoPainter *vplot = ssi_create(VideoPainter, 0, true);
	vplot->getOptions()->setName("video");
	frame->AddConsumer(video_p, vplot, "1");

	plot = ssi_create(SignalPainter, 0, true);
	plot->getOptions()->setName("mouse (tr)");
	plot->getOptions()->type = PaintSignalType::PATH;
	frame->AddEventConsumer(cursor_p, plot, board, zero_trigger->getEventAddress());
	
	frame->Start ();
	board->Start ();
	painter->Arrange (2, 3, 0, 0, 400, 800);
	painter->MoveConsole (400,0,400,800);
	frame->Wait ();
	frame->Stop ();
	board->Stop ();
	frame->Clear ();
	board->Clear ();
	painter->Clear ();

	return true;
}

bool ex_monitor(void *arg) {

	Monitor monitor;
	monitor.setFont("Arial", 12);
	monitor.print("Hello World!");

	ssi::Window window;
	window.setTitle("monitor");
	window.setPosition(ssi_rect(0, 0, 400, 400));
	window.setClient(&monitor);
	window.create();
	window.show();

	ssi_print("\n\n\tpress enter to continue\n");
	getchar();

	monitor.clear();
	monitor.setFont("Courier New", 20);
	monitor.print("Good bye!");
	monitor.update();
	ssi_print("\n\n\tpress enter to continue\n");
	getchar();

	window.close();

	return true;
}
