#include "ssi.h"
#include "emovoice/include/ssiev.h"
#include "audio/include/ssiaudio.h"
#include "graphic/include/ssigraphic.h"
#include "model/include/ssimodel.h"
#include "ssiml.h"
#include "event/include/ZeroEventSender.h"
#include <iostream>
using namespace ssi;

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

void train (const ssi_char_t *dir, const ssi_char_t *model);
void test (const ssi_char_t *file, const ssi_char_t *model);
void stream_test(ssi_char_t *wavfile, ssi_char_t *modelfile);

int main(int argc, char **argv) {
	char info[1024];
	ssi_sprint (info, "\n%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	CmdArgParser cmd;
	cmd.info (info);
	
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssiframe.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssievent.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssiioput.dll");
	//Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssidialog.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssigraphic.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssisignal.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssimodel.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssiaudio.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssiemovoice.dll");
	
	ssi_char_t *dir = 0;
	ssi_char_t *model = "model.model";
	ssi_char_t *wavfile = 0;
	
	cmd.addMasterSwitch("--train");

	cmd.addText("\nArguments:");
	cmd.addSCmdArg("dir", &dir, "directory with training files");
	//cmd.addSCmdArg("model", &model, "filename of model");

	cmd.addMasterSwitch("--test");

	cmd.addText("\nArguments:");
	cmd.addSCmdArg("file", &wavfile, "filename of wave file");
	//cmd.addSCmdArg("model", &model, "filename of model");

	if (cmd.read(argc, argv)) {

		switch (cmd.master_switch) {
		case 1: {
			train(dir, model);
			break;
		}
		case 2: {
			test(wavfile, model);
			break;
		}
		}
	}

	Factory::Clear();

	delete[] dir;
	delete[] model;
	delete[] wavfile;
	return 0;
}

void stream_test(ssi_char_t *wavfile, ssi_char_t *modelfile){
	ITheFramework *frame = Factory::GetFramework();
	IThePainter *painter = Factory::GetPainter();
	ITheEventBoard *board = Factory::GetEventBoard();
	//ssi_pcast(TheFramework, frame)->getOptions()->setConsolePos(0, 400, 300, 400);
	ssi_pcast(TheFramework, frame)->getOptions()->setConsolePos(400,0,400,400);

	WavReader *audio = ssi_create(WavReader, 0, true);
	audio->getOptions()->setPath(wavfile);
	ITransformable *audio_p = frame->AddProvider(audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor(audio);
	/*Audio *audio = ssi_create(Audio, "audio", true);
	audio->getOptions()->remember = true;
	audio->getOptions()->sr = 16000.0;
	audio->getOptions()->channels = 1;
	audio->getOptions()->bytes = 2;
	audio->getOptions()->scale = false;
	ITransformable *audio_p = frame->AddProvider(audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor(audio);*/

	//AudioActivity *vad = ssi_create(AudioActivity, 0, true);
	//vad->getOptions()->method = AudioActivity::LOUDNESS;
	//vad->getOptions()->threshold = 0.05f;
	//ITransformable *vad_t = frame->AddTransformer(audio_p, vad, "0.2s", "0.1s");

	//new EmoVoiceVAD();
	//EmoVoiceVAD *vad = ssi_pcast(EmoVoiceVAD,Factory::Create(EmoVoiceVAD::GetCreateName(), 0));
	//ITransformable *vad_t = frame->AddTransformer(audio_p, vad, "0.2s", "0.1s");

	// mfccs
	//EmoVoiceMFCC *ev_mfcc = ssi_pcast(EmoVoiceMFCC, Factory::Create(EmoVoiceMFCC::GetCreateName(), "ev_feat"));
	//ITransformable *ev_mfcc_t = frame->AddTransformer(audio_p, ev_mfcc, "0.2s");

	// pitch
	//EmoVoicePitch *ev_pitch = ssi_pcast(EmoVoicePitch, Factory::Create(EmoVoicePitch::GetCreateName(), "ev_feat"));
	//ITransformable *ev_pitch_t = frame->AddTransformer(audio_p, ev_pitch, "0.2s", "2.0");

	// trigger
	SNRatio *snratio = ssi_pcast(SNRatio, Factory::Create(SNRatio::GetCreateName()));
	ZeroEventSender *zerotr = ssi_pcast(ZeroEventSender, Factory::Create(ZeroEventSender::GetCreateName()));
	zerotr->getOptions()->mindur = 0.2;
	zerotr->getOptions()->hangin = 3;
	zerotr->getOptions()->hangout = 10;
	frame->AddConsumer(audio_p, zerotr, "0.2s", 0, snratio);
	board->RegisterSender(*zerotr);

	// feature
	EmoVoiceFeat *ev_feat = ssi_pcast(EmoVoiceFeat, Factory::Create(EmoVoiceFeat::GetCreateName(), "ev_feat"));
	ev_feat->getOptions()->maj = 1;

	// classifier
	Trainer ev_class;
	if (!Trainer::Load(ev_class, modelfile)) {
		ssi_err("could not load model");
	}
	Classifier *classifier = ssi_pcast(Classifier, Factory::Create(Classifier::GetCreateName()));
	classifier->setTrainer(&ev_class);
	classifier->getOptions()->console = true;
	frame->AddEventConsumer(audio_p, classifier, board, zerotr->getEventAddress(), ev_feat);

	AudioPlayer *wavplayer = ssi_create(AudioPlayer, 0, true);
	//wavplayer->getOptions()->setPath("audio");
	frame->AddConsumer(audio_p, wavplayer, "0.1s");

	SignalPainter *sigplot = ssi_create(SignalPainter, 0, true);
	sigplot->getOptions()->setName("audio");
	sigplot->getOptions()->size = 10.0;
	sigplot->getOptions()->type = PaintSignalType::AUDIO;
	frame->AddConsumer(audio_p, sigplot, "0.1s");

	sigplot = ssi_pcast(SignalPainter, Factory::Create(SignalPainter::GetCreateName()));
	sigplot->getOptions()->setName("audio (tr)");
	sigplot->getOptions()->type = PaintSignalType::AUDIO;
	frame->AddEventConsumer(audio_p, sigplot, board, zerotr->getEventAddress());

	EventMonitor *monitor = ssi_create(EventMonitor, 0, true);
	monitor->getOptions()->setMonitorPos(300, 400, 300, 400);
	const ssi_char_t *address = "@";
	board->RegisterListener(*monitor, address, 10000);

	frame->Start();
	board->Start();
	//painter->Arrange(1, 2, 0, 0, 600, 400);
	painter->Arrange(1, 3, 0, 0, 400, 800);
	frame->Wait();
	board->Stop();
	frame->Stop();
	frame->Clear();
	board->Clear();
	painter->Clear();
	getchar();
}
void train (const ssi_char_t *dir, const ssi_char_t *model) {

	// load samples
	StringList files;
	FileTools::ReadFilesFromDir (files, dir, "*.wav");
	SampleList samples;	
	samples.addUserName ("user");

	for (ssi_size_t i = 0; i < files.size (); i++) {
		ssi_stream_t *stream = new ssi_stream_t;
		ssi_sample_t *sample = new ssi_sample_t;
		const ssi_char_t *filename = files.get (i);
	
		// parse class name
		FilePath fp (files.get(i));
		ssi_char_t *class_name = ssi_strcpy (fp.getName ());
		for (ssi_size_t j = 0; j < strlen (class_name); j++) {
			if (class_name[j] == '_') {
				class_name[j] = '\0';
				break;
			}
		}
		ssi_size_t class_id = samples.addClassName (class_name);
		delete[] class_name;

		// read wave file
		WavTools::ReadWavFile (filename, *stream);

		// create sample
		sample->class_id = class_id;
		sample->num = 1;
		sample->prob = 1.0f;
		sample->streams = new ssi_stream_t *[1];
		sample->streams[0] = stream;
		sample->time = 0;
		sample->user_id = 0;				

		// add sample
		samples.addSample (sample);
	}

	// extract features
	SampleList samples_t;
	EmoVoiceFeat *ev_feat = ssi_factory_create (EmoVoiceFeat, "ev_feat", true);
	ModelTools::TransformSampleList (samples, samples_t, *ev_feat);
	
	// create model
	IModel *svm_model = ssi_factory_create (EmoVoiceSVM, "svm", true);
	//svm_model->getOptions()->svm_param.svm_type = 1;
	//svm_model->getOptions()->svm_param.kernel_type = 3;
	Trainer trainer (svm_model);

	// evalulation
	Evaluation eval;
	eval.evalKFold (&trainer, samples_t, 10);
	eval.print ();

	// train & save
	trainer.train (samples_t);
	trainer.save (model);
}

void test (const ssi_char_t *file, const ssi_char_t *model) {

	// load model
	Trainer trainer;
	trainer.Load (trainer, model);

	// load file
	ssi_stream_t stream;
	WavTools::ReadWavFile (file, stream);

	// extract features
	ssi_stream_t stream_t;
	EmoVoiceFeat *ev_feat = ssi_factory_create (EmoVoiceFeat, "ev_feat", true);
	SignalTools::Transform (stream, stream_t, *ev_feat, 0);

	// classify
	ssi_size_t class_index;
	trainer.forward (stream_t, class_index);

	ssi_print ("%s\n", trainer.getClassName (class_index));

	ssi_stream_destroy (stream);
	ssi_stream_destroy (stream_t);
	
}