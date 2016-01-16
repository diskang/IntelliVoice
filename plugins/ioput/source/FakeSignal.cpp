// FakeSignal.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2015/01/12
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

#include "FakeSignal.h"

namespace ssi {

FakeSignal::FakeSignal(const ssi_char_t *file)
	: _provider(0),
	_file(0),
	_video_image (0) {
	Thread::setName(getName());

	if (file) {
		if (!OptionList::LoadXML(file, _options)) {
			OptionList::SaveXML(file, _options);
		}
		_file = ssi_strcpy(file);
	}

	ssi_random_seed();
}

FakeSignal::~FakeSignal() {
	if (_file) {
		OptionList::SaveXML(_file, _options);
		delete[] _file;
	}
}

bool FakeSignal::setProvider(const ssi_char_t *name, IProvider *provider) {

	if (_options.type == SIGNAL::IMAGE) {				
		ssi_video_params(_video_params, 640, 480, _options.sr, 8, 1);
		ssi_stream_init(_channel.stream, 0, 1, ssi_video_size (_video_params), SSI_IMAGE, _options.sr);
		provider->setMetaData(sizeof(_video_params), &_video_params);
	} else {
		ssi_stream_init(_channel.stream, 0, 1, sizeof(ssi_real_t), SSI_REAL, _options.sr);
	}

	provider->init(&_channel);
	_provider = provider;
	return true;
}

bool FakeSignal::connect() {
	setClockHz(_channel.stream.sr);
	if (_options.type == SIGNAL::IMAGE) {
		_video_image = new unsigned char[ssi_video_size(_video_params)];
	}
	return true;
}

bool FakeSignal::start() {
	return ClockThread::start(); 	
};

void FakeSignal::clock() {

	if (_options.type == SIGNAL::IMAGE) {

		unsigned char *ptr = _video_image;
		for (int i = 0; i < _video_params.heightInPixels; i++) {
			for (int j = 0; j < _video_params.widthInPixels; j++) {
				*ptr++ = ssi_cast(unsigned char, ssi_random(0, 255));
			}
			ptr = _video_image + i * ssi_video_stride(_video_params);
		}
		_provider->provide(ssi_pcast(ssi_byte_t, _video_image), 1);

	} else {

		ssi_real_t y = 0;

		if (_options.type == SIGNAL::RANDOM) {
			y = ssi_cast(ssi_real_t, ssi_random());
		}
		else {
			y = sin(2.0f*3.1416f*(ssi_time_ms() / 1000.0f));
		}

		_provider->provide(ssi_pcast(ssi_byte_t, &y), 1);
	}
}

bool FakeSignal::stop() {
	return ClockThread::stop();	
};

bool FakeSignal::disconnect() {
	delete _video_image; _video_image = 0;
	return true;
}
}