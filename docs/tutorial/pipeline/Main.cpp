// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/04/24
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
#include "MyConsumer.h"
#include "MySensor.h"
#include "MyTransformer.h"
#include "MyFilter.h"
#include "MyFilter2.h"
#include "MyFeature.h"
#include "MyFeature2.h"
#include "MyEventSender.h"
#include "MyEventListener.h"
using namespace ssi;

bool ex_pipeline (void *args);
bool ex_ioput(void *args);
bool ex_transf(void *args);
bool ex_filter(void *args);
bool ex_feature(void *args);
bool ex_chain(void *args);
bool ex_event(void *args);

int main () {

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe");
	Factory::RegisterDLL ("ssievent");
	Factory::RegisterDLL ("ssisignal");
	Factory::RegisterDLL ("ssiioput");
	Factory::RegisterDLL ("ssigraphic");

	Factory::Register (MySensor::GetCreateName (), MySensor::Create);
	Factory::Register (MyConsumer::GetCreateName (), MyConsumer::Create);
	Factory::Register (MyTransformer::GetCreateName (), MyTransformer::Create);
	Factory::Register (MyFilter::GetCreateName (), MyFilter::Create);
	Factory::Register (MyFilter2::GetCreateName (), MyFilter2::Create);
	Factory::Register (MyFeature::GetCreateName (), MyFeature::Create);
	Factory::Register (MyFeature2::GetCreateName (), MyFeature2::Create);
	Factory::Register (MyEventSender::GetCreateName (), MyEventSender::Create);
	Factory::Register (MyEventListener::GetCreateName (), MyEventListener::Create);

	Exsemble exsemble;
	exsemble.add(ex_pipeline, 0, "PIPELINE", "run a pipeline");
	exsemble.add(ex_ioput, 0, "IOPUT", "output a stream");
	exsemble.add(ex_transf, 0, "TRANSFORMER", "transform a stream");
	exsemble.add(ex_filter, 0, "FILTER", "filter a stream");
	exsemble.add(ex_feature, 0, "FEATURE", "extract a feature");
	exsemble.add(ex_chain, 0, "CHAIN", "create a transformation chain");
	exsemble.add(ex_event, 0, "EVENT", "send an receive events");
	exsemble.show();

	Factory::Clear ();

	return 0;
}

bool ex_pipeline(void *args) {

	ITheFramework *frame = Factory::GetFramework ();
	
	MySensor *sensor = ssi_create (MySensor, "sensor", true);
	sensor->getOptions ()->sr = 5.0;
	ITransformable *sensor_p = frame->AddProvider (sensor, MYSENSOR_PROVIDER_NAME);
	frame->AddSensor (sensor);

	MyConsumer *consumer = ssi_create (MyConsumer, 0, true);
	frame->AddConsumer (sensor_p, consumer, "0.5s");		

	frame->Start ();
	frame->Wait ();
	frame->Stop ();
	frame->Clear ();

	return true;
}

bool ex_ioput(void *args) {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
	
	MySensor *sensor = ssi_create (MySensor, "sensor", true);
	sensor->getOptions ()->sr = 5.0;
	ITransformable *sensor_p = frame->AddProvider (sensor, MYSENSOR_PROVIDER_NAME);
	frame->AddSensor (sensor);

	FileWriter *filewrite = ssi_create (FileWriter, 0, true);
	filewrite->getOptions ()->type = File::ASCII;
	filewrite->getOptions ()->setPath ("cursor.txt");
	frame->AddConsumer (sensor_p, filewrite, "0.5s");	

	SocketWriter *sockwrite = ssi_create (SocketWriter, 0, true);
	sockwrite->getOptions ()->port = 1111;
	sockwrite->getOptions ()->setHost ("localhost");
	sockwrite->getOptions ()->type = Socket::UDP;
	frame->AddConsumer (sensor_p, sockwrite, "0.5s");

	SignalPainter *sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("cursor");
	sigpaint->getOptions ()->size = 10.0;
	sigpaint->getOptions ()->setMove (0, 0, 300, 300);
	frame->AddConsumer (sensor_p, sigpaint, "0.5s");
	
	frame->Start ();
	frame->Wait ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();

	return true;
}

bool ex_transf(void *args) {

	ITheFramework *frame = Factory::GetFramework ();
	
	MySensor *sensor = ssi_create (MySensor, 0, true);
	ITransformable *sensor_p = frame->AddProvider (sensor, MYSENSOR_PROVIDER_NAME);
	frame->AddSensor (sensor);

	MyTransformer *transf = ssi_create (MyTransformer, 0, true);
	ITransformable *transf_t = frame->AddTransformer (sensor_p, transf, "0.5s");	

	ITransformable *source[] = { sensor_p, transf_t };
	MyConsumer *consumer = ssi_create (MyConsumer, 0, true);
	frame->AddConsumer (2, source, consumer, "0.5s");

	frame->Start ();
	frame->Wait();
	frame->Stop ();
	frame->Clear ();

	return true;
}

bool ex_filter(void *args) {

	ITheFramework *frame = Factory::GetFramework ();
	
	MySensor *sensor = ssi_create (MySensor, 0, true);
	ITransformable *sensor_p = frame->AddProvider (sensor, MYSENSOR_PROVIDER_NAME);
	frame->AddSensor (sensor);

	MyFilter *filter = ssi_create (MyFilter, 0, true);
	ITransformable *filter_t = frame->AddTransformer (sensor_p, filter, "0.5s");	

	ITransformable *source[] = { sensor_p, filter_t };
	MyConsumer *consumer = ssi_create (MyConsumer, 0, true);
	frame->AddConsumer (2, source, consumer, "0.5s");

	frame->Start ();
	frame->Wait ();
	frame->Stop ();
	frame->Clear ();

	return true;
}

bool ex_feature(void *args) {

	ITheFramework *frame = Factory::GetFramework ();
	
	MySensor *sensor = ssi_create (MySensor, 0, true);
	ITransformable *sensor_p = frame->AddProvider (sensor, MYSENSOR_PROVIDER_NAME);
	frame->AddSensor (sensor);

	MyFeature *feature = ssi_create (MyFeature, 0, true);
	ITransformable *feature_t = frame->AddTransformer (sensor_p, feature, "0.5s");

	MyFeature2 *feature2 = ssi_create (MyFeature2, 0, true);
	ITransformable *feature2_t = frame->AddTransformer (sensor_p, feature2, "0.5s");

	ITransformable *source[] = { sensor_p, feature_t, feature2_t };
	MyConsumer *consumer = ssi_create (MyConsumer, 0, true);
	frame->AddConsumer (3, source, consumer, "0.5s");	

	frame->Start ();
	frame->Wait ();
	frame->Stop ();
	frame->Clear ();

	return true;
}

bool ex_chain(void *args) {

	ITheFramework *frame = Factory::GetFramework ();
	
	MySensor *sensor = ssi_create (MySensor, 0, true);
	ITransformable *sensor_p = frame->AddProvider (sensor, MYSENSOR_PROVIDER_NAME);
	frame->AddSensor (sensor);

	MyFilter *filter = ssi_create (MyFilter, 0, true);
	MyFeature *feature = ssi_create (MyFeature, 0, true);
	MyFeature2 *feature2 = ssi_create (MyFeature2, 0, true);
	IFilter *filters[1] = {filter};
	IFeature *features[2] = {feature, feature2};
	Chain *chain = ssi_factory_create (Chain, 0, true);
	chain->set (1, filters, 2, features);
	ITransformable *chain_t = frame->AddTransformer (sensor_p, chain, "0.5s");	

	MyConsumer *printer = ssi_create (MyConsumer, 0, true);
	frame->AddConsumer (sensor_p, printer, "0.5s");	

	MyConsumer *printer_t = ssi_create (MyConsumer, 0, true);
	frame->AddConsumer (chain_t, printer_t, "0.5s");

	frame->Start ();
	frame->Wait ();
	frame->Stop ();
	frame->Clear ();

	return true;
}

bool ex_event(void *args) {

	ITheFramework *frame = Factory::GetFramework ();
	ITheEventBoard *board = Factory::GetEventBoard ();
	
	MySensor *sensor = ssi_create (MySensor, 0, true);
	ITransformable *sensor_p = frame->AddProvider (sensor, MYSENSOR_PROVIDER_NAME);
	frame->AddSensor (sensor);

	MyEventSender *sender = ssi_create (MyEventSender, 0, true);
	frame->AddConsumer (sensor_p, sender, "2.5s");
	board->RegisterSender (*sender);

	MyEventListener *listener = ssi_create (MyEventListener, 0, true);
	board->RegisterListener (*listener, sender->getEventAddress ());	

	frame->Start ();
	board->Start ();
	frame->Wait ();
	board->Stop ();
	frame->Stop ();	
	board->Clear ();
	frame->Clear ();	

	return true;
}
