// ElanTools.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/03/04
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

#include "ElanTools.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t *ElanTools::ssi_log_name = "elantools_";

void ElanTools::LoadSampleList (SampleList &samples,		
	ssi_stream_t &stream,
	ElanTier &elanTier,
	const ssi_char_t *user_name,
	bool useTierNameAsLabel) {

	ssi_stream_t *s = &stream;
	ElanTools::LoadSampleList (samples, 1, &s, elanTier, user_name, useTierNameAsLabel);
}

void ElanTools::LoadSampleList (SampleList &samples,
	ssi_size_t num,
	ssi_stream_t *streams[],
	ElanTier &elanTier,
	const ssi_char_t *user_name,
	bool useTierNameAsLabel) {

	// add user name
	ssi_size_t user_id = samples.addUserName (user_name);

	// add labels
	ssi_size_t class_id;
	if (useTierNameAsLabel) {		
		class_id = samples.addClassName (elanTier.name ());
	} 

	// add samples
	ElanTier::iterator anno;		
	for (anno = elanTier.begin (); anno != elanTier.end (); anno++) {
	
		ssi_sample_t *sample = new ssi_sample_t;
		ssi_stream_t **chops = new ssi_stream_t *[num];

		bool success = false;
		for (ssi_size_t j = 0; j < num; j++) {	

			// calculate start and stop index
			ssi_size_t start_index = ssi_cast (ssi_size_t, (anno->from / 1000.0) * streams[j]->sr + 0.5);
			ssi_size_t stop_index = ssi_cast (ssi_size_t, (anno->to / 1000.0) * streams[j]->sr + 0.5);

			if (! (start_index <= stop_index && stop_index < streams[j]->num)) {
				ssi_wrn ("invalid interval [%lf..%lf]s", anno->from/1000.0, anno->to/1000.0);
				continue;
			}

			// extract sample			
			chops[j] = new ssi_stream_t;
 			ssi_stream_copy (*streams[j], *chops[j], start_index, stop_index);

			success = true;
		}

		if (success) {

			// create and add new sample
			if (useTierNameAsLabel) {
				sample->class_id = class_id;
			} else {
				sample->class_id = samples.addClassName (anno->value.str ());
			}
			sample->num = num;
			sample->prob = 1.0f;
			sample->streams = chops;
			sample->time = anno->from / 1000.0;
			sample->user_id = user_id;
			samples.addSample (sample);

		} else {
			delete sample;
			delete[] chops;
		}

	}
}

}
