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
#include "MyModel.h"
#include "MyFusion.h"
#include "audio\include\ssiaudio.h"
#include "mouse\include\ssimouse.h"
#include "ssiml.h"
#include "model\include\ssimodel.h"
using namespace ssi;

#define N_COLORS 5
unsigned short COLORS[][3] = {
	128,0,0,
	0,128,0,
	0,0,128,
	128,0,128,
	0,128,128
};

bool ex_model (void *args);
bool ex_fusion(void *args);
bool ex_online(void *args);
void PlotSamples(ISamples &samples, const ssi_char_t *name);

int main () {

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe");
	Factory::RegisterDLL ("ssievent");
	Factory::RegisterDLL ("ssimodel");
	Factory::RegisterDLL ("ssimouse");
	Factory::RegisterDLL ("ssigraphic");

	Factory::Register (MyModel::GetCreateName (), MyModel::Create);
	Factory::Register (MyFusion::GetCreateName (), MyFusion::Create);

	ssi_random_seed ();

	Exsemble exsemble;
	exsemble.add(ex_model, 0, "MODEL", "train a model");	
	exsemble.add(ex_fusion, 0, "FUSION", "train a fusion model");
	exsemble.add(ex_online, 0, "ONLINE", "do online classification");
	exsemble.show();

	Factory::Clear ();

	return 0;
}

bool ex_model(void *args) {

	ssi_size_t n_classes = 4;
	ssi_size_t n_sampels = 50;
	ssi_size_t n_streams = 1;

	SampleList strain, sdevel;
	{
		ssi_real_t distr[][3] = { 0.33f, 0.33f, 0.15f, 0.33f, 0.66f, 0.15f, 0.66f, 0.33f, 0.15f, 0.66f, 0.66f, 0.15f};
		ModelTools::CreateTestSamples (strain, n_classes, n_sampels, n_streams, distr);	
		ModelTools::CreateTestSamples (sdevel, n_classes, n_sampels, n_streams, distr);
	}

	SampleList stest;
	{
		ssi_real_t distr[][3] = { 0.5f, 0.5f, 0.5f };
		ModelTools::CreateTestSamples (stest, 1, n_sampels * n_classes, n_streams, distr);	
		ssi_char_t string[SSI_MAX_CHAR];	
		for (ssi_size_t n_class = 1; n_class < n_classes; n_class++) {
			ssi_sprint (string, "class%02d", n_class);
			stest.addClassName (string);
		}
	}

	{
		MyModel *model = ssi_create (MyModel, MyModel::GetCreateName (), 0);

		Trainer trainer (model, 0);
		trainer.train (strain);
		trainer.save ("mymodel");
	}

	{
		Trainer trainer;
		Trainer::Load (trainer, "mymodel");

		Evaluation eval;
		eval.eval (&trainer, sdevel);
		eval.print ();
		
		PlotSamples (strain, "training samples");
		PlotSamples (stest, "test samples");
		trainer.cluster (stest);
		PlotSamples (stest, "clustered test samples");
	}

	return true;
}

bool ex_fusion(void *args) {

	ssi_size_t n_classes = 4;
	ssi_size_t n_sampels = 50;
	ssi_size_t n_streams = 5;

	SampleList strain, sdevel;
	{
		ssi_real_t distr[][3] = { 0.33f, 0.33f, 0.15f, 0.33f, 0.66f, 0.15f, 0.66f, 0.33f, 0.15f, 0.66f, 0.66f, 0.15f};
		ModelTools::CreateTestSamples (strain, n_classes, n_sampels, n_streams, distr);	
		ModelTools::CreateTestSamples (sdevel, n_classes, n_sampels, n_streams, distr);
	}

	{
		IModel **models = new IModel *[n_streams];
		for (ssi_size_t i = 0; i < n_streams; i++) {
			models[i] = ssi_create(MyModel, 0, true);
		}
		MyFusion *fusion = ssi_create(MyFusion, 0, true);

		Trainer trainer (n_streams, models, fusion);
		trainer.train (strain);
		trainer.save ("myfusion");
	}

	{
		Trainer trainer;
		Trainer::Load (trainer, "myfusion");

		Evaluation eval;
		eval.eval (&trainer, sdevel);
		eval.print ();
	}

	return true;
}

bool ex_online(void *args) {

	ITheFramework *frame = Factory::GetFramework ();

	ssi_size_t n_classes = 4;
	ssi_size_t n_sampels = 50;
	ssi_size_t n_streams = 1;
	ssi_real_t distr[][3] = { 0.66f, 0.33f, 0.15f, 0.33f, 0.33f, 0.15f, 0.66f, 0.66f, 0.15f, 0.33f, 0.66f, 0.15f };
	SampleList strain;
	ModelTools::CreateTestSamples (strain, n_classes, n_sampels, n_streams, distr);	

	MyModel *model = ssi_create(MyModel, 0, true);
	Trainer trainer (model, 0);
	trainer.train (strain);
	
	Mouse *mouse = ssi_create(Mouse, 0, true);
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	frame->AddSensor (mouse); 
				
	Classifier *classifier = ssi_create(Classifier, 0, true);
	classifier->setTrainer (&trainer);
	frame->AddConsumer (cursor_p, classifier, "0.5s");

	frame->Start ();
	getchar ();
	frame->Stop ();
	frame->Clear ();

	return true;
}

void PlotSamples(ISamples &samples, const ssi_char_t *name) {

	IThePainter *painter = Factory::GetPainter();

	int plot_id = painter->AddCanvas(name);

	ssi_size_t n_classes = samples.getClassSize();
	PaintData **plots = new PaintData *[n_classes];

	for (ssi_size_t n_class = 0; n_class < n_classes; n_class++) {

		plots[n_class] = new PaintData;

		ssi_size_t n_samples = samples.getSize(n_class);
		ssi_size_t n_streams = samples.getStreamSize();

		ssi_stream_t stream;
		ssi_stream_init(stream, n_samples * n_streams, 2, sizeof(ssi_real_t), SSI_REAL, 0);

		ssi_real_t *data = ssi_pcast(ssi_real_t, stream.ptr);

		ISSelectClass samples_sel(&samples);
		samples_sel.setSelection(n_class);
		samples_sel.reset();
		ssi_sample_t *sample = 0;
		ssi_real_t *data_ptr = data;
		while (sample = samples_sel.next()) {
			for (ssi_size_t n_stream = 0; n_stream < n_streams; n_stream++) {
				memcpy(data_ptr, sample->streams[n_stream]->ptr, 2 * sizeof(ssi_real_t));
				data_ptr += 2;
			}
		}

		plots[n_class]->setBackground(n_class == 0, IPainter::ITool::COLORS::BLACK);
		plots[n_class]->setPointSize(10);
		plots[n_class]->setLimits(-0.2f, 1.2f);
		plots[n_class]->setData(stream, PaintData::TYPE::SCATTER);
		plots[n_class]->setBrush(ssi_rgb(COLORS[n_class][0], COLORS[n_class][1], COLORS[n_class][2]));
		plots[n_class]->setPen(IPainter::ITool::COLORS::BLACK);
		painter->AddObject(plot_id, plots[n_class]);

		ssi_stream_destroy(stream);
	}

	painter->Update(plot_id);
	painter->Move(plot_id, 0, 0, 600, 600);

	ssi_print("\n\n\tpress enter to continue\n\n");
	getchar();

	for (ssi_size_t n_class = 0; n_class < n_classes; n_class++) {
		delete plots[n_class];
	}
	delete[] plots;

	painter->Clear();
}
