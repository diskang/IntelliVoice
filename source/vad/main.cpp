#include "ssi.h"
#include "audio/include/ssiaudio.h"
#include "graphic/include/ssigraphic.h"
#include "event/include/ZeroEventSender.h"
using namespace ssi;

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

int main() {
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssiframe.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssievent.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssiioput.dll");
	//Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssidialog.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssigraphic.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssisignal.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssimodel.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssiaudio.dll");
	Factory::RegisterDLL("E:\\code\\intelliVoice\\bin\\x64\\vc140\\ssiemovoice.dll");
	
	ITheFramework *frame = Factory::GetFramework();
	IThePainter *painter = Factory::GetPainter();
	ITheEventBoard *board = Factory::GetEventBoard();
	ssi_pcast(TheFramework, frame)->getOptions()->setConsolePos(0, 400, 300, 400);


	WavReader *audio = ssi_create(WavReader, 0, true);
	audio->getOptions()->setPath("E:\\DIT\\kaiti\\data\\lubo\\chemical.wav");
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
	//std::cin >> i;
	/*<transformer create="ssi_feature_AudioActivity" method="0" threshold="0.05">
		<input pin="audio" frame="0.03s" delta="0.015s"/>
		<output pin="audio_vad"/>
	</transformer>*/
	AudioActivity *vad = ssi_create(AudioActivity, 0, true);
	vad->getOptions()->method = AudioActivity::LOUDNESS;
	vad->getOptions()->threshold = 0.05f;
	ITransformable *vad_t = frame->AddTransformer(audio_p, vad, "0.03s", "0.015s");

	ZeroEventSender *ezero = ssi_create(ZeroEventSender, 0, true);
	ezero->getOptions()->mindur = 0.2;
	ezero->getOptions()->hangin = 3;
	ezero->getOptions()->hangout = 10;
	frame->AddConsumer(vad_t, ezero, "0.1s", "0");
	board->RegisterSender(*ezero);

	WavWriter *wavwrite = ssi_create(WavWriter, 0, true);
	wavwrite->getOptions()->setPath("audio");
	frame->AddConsumer(audio_p, wavwrite, "0.1s");

	SignalPainter *sigplot = ssi_create(SignalPainter, 0, true);
	sigplot->getOptions()->setName("audio");
	sigplot->getOptions()->size = 10.0;
	sigplot->getOptions()->type = PaintSignalType::AUDIO;
	frame->AddConsumer(audio_p, sigplot, "0.1s");

	sigplot = ssi_create(SignalPainter, 0, true);
	sigplot->getOptions()->setName("activity");
	sigplot->getOptions()->size = 10.0;
	frame->AddConsumer(vad_t, sigplot, "0.1s");

	EventMonitor *monitor = ssi_create(EventMonitor, 0, true);
	monitor->getOptions()->setMonitorPos(300, 400, 300, 400);
	const ssi_char_t *address = "@";
	board->RegisterListener(*monitor, address, 10000);

	frame->Start();
	board->Start();
	painter->Arrange(1, 2, 0, 0, 600, 400);
	frame->Wait();
	board->Stop();
	frame->Stop();
	frame->Clear();
	board->Clear();
	painter->Clear();
	return 0;
}