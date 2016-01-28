// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/10/11
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
#include "ssiml.h"
#include "ssimodel.h"
#include "signal/include/ssisignal.h"
using namespace ssi;

#define N_COLORS 7
unsigned short COLORS[][3] = {
	128,0,0,
	0,128,0,
	0,0,128,
	128,0,128,
	0,128,128,
	255,128,0,
	0,128,255
};

bool ex_random (void *arg);
bool ex_samplelist(void *arg);
bool ex_eval(void *arg);
bool ex_model(void *arg);
bool ex_fusion(void *arg);
bool ex_hierarchical(void *arg);

void PlotSamples (ISamples &samples, const ssi_char_t *name);
void CreateMissingData (SampleList &samples, double prob);

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

int main () {	

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssimodel.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssisignal.dll");

	ssi_random_seed ();

	Factory::GetPainter()->MoveConsole(0, 600, 600, 400);

	Exsemble exsemble;
	exsemble.add(&ex_random, 0, "RANDOM", "Test of random number generator.");	
	exsemble.add(&ex_eval, 0, "EVALUATION", "How to do an evaluation.");
	exsemble.add(&ex_model, 0, "MODEL", "How to train a single model.");
	exsemble.add(&ex_hierarchical, 0, "HIERARCHICAL", "How to train a hierarchical model.");
	exsemble.add(&ex_fusion, 0, "FUSION", "How to train a fusion model.");	
	exsemble.show();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

bool ex_random(void *arg) {

	unsigned int seed = ssi_cast (unsigned int, time (NULL));
	ssi_size_t n = 100000;

	ssi_tic ();
	for (ssi_size_t i = 0; i < n; i++) {
		ssi_random ();
	}
	ssi_toc_print ();

	ssi_print ("\n");

	ssi_tic ();
	for (ssi_size_t i = 0; i < n; i++) {
		ssi_random_distr (0,1);
	}
	ssi_toc_print ();

	ssi_print ("\n");

	return true;
}

bool ex_eval(void *arg) {

	ssi_size_t n_classes = 2;
	ssi_size_t n_samples = 20;
	ssi_size_t n_streams = 1;
	ssi_real_t train_distr[][3] = { 0.3f, 0.3f, 0.2f, 0.3f, 0.6f, 0.2f, 0.6f, 0.3f, 0.2f, 0.6f, 0.6f, 0.2f };
	ssi_real_t test_distr[][3] = { 0.5f, 0.5f, 0.5f };
	SampleList samples;		
	ModelTools::CreateTestSamples (samples, n_classes, n_samples, n_streams, train_distr);	
	ssi_char_t string[SSI_MAX_CHAR];	
	for (ssi_size_t n_class = 1; n_class < n_classes; n_class++) {
		ssi_sprint (string, "class%02d", n_class);
		samples.addClassName (string);
	}

	Evaluation eval;
	NaiveBayes *model = ssi_pcast (NaiveBayes, Factory::Create (NaiveBayes::GetCreateName ()));
	Trainer trainer (model);
	trainer.train (samples);

	Evaluation2Latex e2latex;
	e2latex.open ("eval.tex");
	
	ssi_print ("devel set:\n");
	eval.eval (&trainer, samples);
	eval.print ();
	eval.print_result_vec ();

	e2latex.writeHead (eval, "caption", "label");
	e2latex.writeText ("results with different evaluation strategies", true);
	e2latex.writeEval ("devel", eval);
	
	ssi_print ("k-fold:\n");
	eval.evalKFold (&trainer, samples, 3); 
	eval.print ();
	eval.print_result_vec ();

	e2latex.writeEval ("k-fold", eval);

	ssi_print ("split:\n");
	eval.evalSplit (&trainer, samples, 0.5f); 
	eval.print ();
	eval.print_result_vec ();

	e2latex.writeEval ("split", eval);

	ssi_print ("loo:\n");
	eval.evalLOO (&trainer, samples); 
	eval.print ();
	eval.print_result_vec ();

	e2latex.writeEval ("loo", eval);
	
	e2latex.writeTail ();
	e2latex.close ();

	FILE *fp = fopen("eval.csv", "w");
	eval.print(fp, Evaluation::PRINT::CSV);
	fclose(fp);

	return true;
}

bool ex_model(void *arg) {

	Trainer::SetLogLevel (SSI_LOG_LEVEL_DEBUG);

	ssi_size_t n_classes = 4;
	ssi_size_t n_samples = 50;
	ssi_size_t n_streams = 1;
	ssi_real_t train_distr[][3] = { 0.25f, 0.25f, 0.1f, 0.25f, 0.75f, 0.1f, 0.75f, 0.75f, 0.1f, 0.75f, 0.75f, 0.1f };
	ssi_real_t test_distr[][3] = { 0.5f, 0.5f, 0.5f };
	SampleList strain;
	SampleList sdevel;
	SampleList stest;
	ModelTools::CreateTestSamples (strain, n_classes, n_samples, n_streams, train_distr, "user");	
	ModelTools::CreateTestSamples (sdevel, n_classes, n_samples, n_streams, train_distr, "user");	
	ModelTools::CreateTestSamples (stest, 1, n_samples * n_classes, n_streams, test_distr, "user");	
	ssi_char_t string[SSI_MAX_CHAR];	
	for (ssi_size_t n_class = 1; n_class < n_classes; n_class++) {
		ssi_sprint (string, "class%02d", n_class);
		stest.addClassName (string);
	}

	// train knn
	{
		KNearestNeighbors *model = ssi_pcast (KNearestNeighbors, Factory::Create (KNearestNeighbors::GetCreateName ()));
		model->getOptions ()->k = 5;
		//model->getOptions ()->distsum = true;
		Trainer trainer (model);
		trainer.train (strain);
		trainer.save ("out\\knn");
	}

	// evaluation
	{
		Trainer trainer;
		Trainer::Load (trainer, "out\\knn");			
		Evaluation eval;
		eval.eval (&trainer, sdevel);
		eval.print ();

		trainer.cluster (stest);
		PlotSamples (stest, "knn");
	}

	// train naive bayes
	{
		NaiveBayes *model = ssi_pcast (NaiveBayes, Factory::Create (NaiveBayes::GetCreateName ()));
		model->getOptions ()->log = true;
		Trainer trainer (model);
		trainer.train (strain);
		trainer.save ("out\\bayes");
	}

	// evaluation
	{
		Trainer trainer;
		Trainer::Load (trainer, "out\\bayes");			
		Evaluation eval;
		eval.eval (&trainer, sdevel);
		eval.print ();

		trainer.cluster (stest);
		PlotSamples (stest, "bayes");
	}

	// train svm
	{
		SVM *model = ssi_pcast (SVM, Factory::Create (SVM::GetCreateName ()));
		Trainer trainer (model);
		trainer.train (strain);
		trainer.save ("out\\svm");
	}

	// evaluation
	{
		Trainer trainer;
		Trainer::Load (trainer, "out\\svm");			
		Evaluation eval;
		eval.eval (&trainer, sdevel);
		eval.print ();

		trainer.cluster (stest);
		PlotSamples (stest, "svm");
	}

	// training
	{
		LDA *model = ssi_factory_create (LDA, "lda", true);		
		Trainer trainer (model);
		trainer.train (strain);

		model->print ();
		trainer.save ("out\\lda");
	}

	// evaluation
	{
		Trainer trainer;
		Trainer::Load (trainer, "out\\lda");
		Evaluation eval;
		eval.eval (&trainer, sdevel);
		eval.print ();

		trainer.cluster (stest);
		PlotSamples (stest, "lda");
	}

	ssi_print ("\n\n\tpress a key to contiue\n");
	getchar ();

	return true;
}

bool ex_hierarchical(void *arg) {

	Trainer::SetLogLevel(SSI_LOG_LEVEL_DEBUG);

	ssi_size_t n_classes = 6;
	ssi_size_t n_samples = 50;
	ssi_size_t n_streams = 1;
	ssi_real_t train_distr[][3] = { 0.2f, 0.2f, 0.1f, 
									0.2f, 0.5f, 0.1f,
									0.2f, 0.8f, 0.1f, 
									0.8f, 0.8f, 0.1f, 
									0.8f, 0.5f, 0.1f,
									0.8f, 0.2f, 0.1f,									
	};
	ssi_real_t test_distr[][3] = { 0.5f, 0.5f, 0.5f };
	SampleList strain;
	SampleList sdevel;
	SampleList stest;
	ModelTools::CreateTestSamples(strain, n_classes, n_samples, n_streams, train_distr, "user");
	ModelTools::CreateTestSamples(sdevel, n_classes, n_samples, n_streams, train_distr, "user");
	ModelTools::CreateTestSamples(stest, 1, n_samples * n_classes, n_streams, test_distr, "user");
	ssi_char_t string[SSI_MAX_CHAR];
	for (ssi_size_t n_class = 1; n_class < n_classes; n_class++) {
		ssi_sprint(string, "class%02d", n_class);
		stest.addClassName(string);
	}

	PlotSamples(strain, "train");

	// non-hierarchical
	{
		SVM *model = ssi_create(SVM, 0, true);
		model->getOptions()->params.kernel_type = LINEAR;
		model->getOptions()->balance = SVM::BALANCE::OFF;
		Trainer trainer(model);
		trainer.train(strain);
		trainer.eval(sdevel);
		trainer.cluster(stest);
		PlotSamples(stest, "svm");
		OptionList::SaveXML("svm", *model->getOptions());
	}

	// hierarchical
	{
		HierarchicalModel *hmodel = ssi_create(HierarchicalModel, 0, true);

		/*
		hmodel->initTree (5);
		hmodel->addNode(0, 0, ssi_create(SVM, "svm", true), "0, 1, 2, 3, 4, 5");		
		
		hmodel->addNode(1, 0, ssi_create(SVM, "svm", true), "0, 1, 2, 3, 4");
		hmodel->addNode(1, 1, ssi_create(SVM, "svm", true), "5");

		hmodel->addNode(2, 0, ssi_create(SVM, "svm", true), "0, 1, 2");
		hmodel->addNode(2, 1, ssi_create(SVM, "svm", true), "3, 4");

		hmodel->addNode(3, 0, ssi_create(SVM, "svm", true), "0, 1");
		hmodel->addNode(3, 1, ssi_create(SVM, "svm", true), "2");
		*/

		hmodel->initTree(2);
		hmodel->addNode(0, 0, ssi_create(SVM, "svm", true), "0, 1, 2, 3, 4, 5");

		hmodel->addNode(1, 0, ssi_create(SVM, "svm", true), "0, 1, 2", "1");
		hmodel->addNode(1, 1, ssi_create(SVM, "svm", true), "3, 4, 5", "1");

		hmodel->getTree()->print(HierarchicalModel::ToString);

		Trainer trainer(hmodel);
		trainer.train(strain);
		trainer.save("hierarchical");		
	}

	{
		Trainer trainer;
		Trainer::Load(trainer, "hierarchical");
		trainer.eval(sdevel);

		trainer.cluster(stest);
		PlotSamples(stest, "hierarchical svm");
	}

	return true;
}

bool ex_fusion(void *arg) {

	ssi_tic ();

	ssi_size_t n_classes = 4;
	ssi_size_t n_samples = 50;
	ssi_size_t n_streams = 3;
	ssi_real_t train_distr[][3] = { 0.25f, 0.25f, 0.1f, 0.25f, 0.75f, 0.1f, 0.75f, 0.75f, 0.1f, 0.75f, 0.75f, 0.1f };
	ssi_real_t test_distr[][3] = { 0.5f, 0.5f, 0.5f };
	SampleList strain;
	SampleList sdevel;
	SampleList stest;
	ModelTools::CreateTestSamples (strain, n_classes, n_samples, n_streams, train_distr, "user");			
	ModelTools::CreateTestSamples (sdevel, n_classes, n_samples, n_streams, train_distr, "user");	
	ModelTools::CreateTestSamples (stest, 1, n_samples * n_classes, n_streams, test_distr, "user");	
	ssi_char_t string[SSI_MAX_CHAR];	
	for (ssi_size_t n_class = 1; n_class < n_classes; n_class++) {
		ssi_sprint (string, "class%02d", n_class);
		stest.addClassName (string);
	}

	ssi_char_t *name = "fusion";

	// strain
	{
		IModel **models = new IModel *[n_streams];
		ssi_char_t string[SSI_MAX_CHAR];
		for (ssi_size_t n_stream = 0; n_stream < n_streams; n_stream++) {
			ssi_sprint (string, "%s.%02d", name, n_stream);
			models[n_stream] = ssi_pcast (IModel, Factory::Create (SimpleKNN::GetCreateName (), string));
		}
		SimpleFusion *fusion = ssi_pcast (SimpleFusion, Factory::Create (SimpleFusion::GetCreateName (), name));

		Trainer trainer (n_streams, models, fusion);
		trainer.train (strain);
		trainer.save ("out\\fusion");

		delete[] models;
	}

	// evaluation
	{
		Trainer trainer;
		Trainer::Load (trainer, "out\\fusion");					
		Evaluation eval;
		eval.eval (&trainer, sdevel);
		eval.print ();
	}

	ssi_toc_print ();

	return true;
}


void CreateMissingData (SampleList &samples, double prob) {

	ssi_size_t n_streams = samples.getStreamSize ();
	ssi_sample_t *sample = 0;
	samples.reset ();
	while (sample = samples.next ()) {
		for (ssi_size_t nstrm = 0; nstrm < n_streams; nstrm++) {
			if (ssi_random () > prob) {							
				ssi_stream_reset (*sample->streams[nstrm]);
			}
		}
	}
	samples.setMissingData (true);
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

