// TupleEventSender.cpp
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

#include "TupleEventSender.h"
#include "base/Factory.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t *TupleEventSender::ssi_log_name = "tuplesend_";

TupleEventSender::TupleEventSender (const ssi_char_t *file)
	: _file (0),
	_listener (0),
	_n_tuple(0),
	ssi_log_level (SSI_LOG_LEVEL_DEFAULT) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}

	ssi_event_init (_event, SSI_ETYPE_NTUPLE);
}

TupleEventSender::~TupleEventSender () {

	ssi_event_destroy (_event);

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

bool TupleEventSender::setEventListener (IEventListener *listener) {

	_listener = listener;
	_event.sender_id = Factory::AddString (_options.sname);
	if (_event.sender_id == SSI_FACTORY_STRINGS_INVALID_ID) {
		return false;
	}
	_event.event_id = Factory::AddString (_options.ename);
	if (_event.event_id == SSI_FACTORY_STRINGS_INVALID_ID) {
		return false;
	}
	
	_event_address.setSender (_options.sname);
	_event_address.setEvents (_options.ename);	

	return true;
}

void TupleEventSender::consume_enter (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	if (stream_in[0].type != SSI_REAL) {
		ssi_err ("type '%s' not supported", SSI_TYPE_NAMES[stream_in[0].type]);
	}

	ssi_size_t dim = stream_in[0].dim;

	_n_tuple = 0;
	if (_options.mean) _n_tuple++;
	if (_options.var) _n_tuple++;
	if (_options.minval) _n_tuple++;
	if (_options.maxval) _n_tuple++;

	ssi_event_adjust(_event, _n_tuple * sizeof(ssi_event_tuple_t) * dim);
	ssi_event_tuple_t *dst = ssi_pcast(ssi_event_tuple_t, _event.ptr);

	if (_options.mean) {
		ssi_size_t id = Factory::AddString("mean");
		for (ssi_size_t i = 0; i < dim; i++) {
			dst++->id = id;
		}
	}
	if (_options.var) {
		ssi_size_t id = Factory::AddString("var");
		for (ssi_size_t i = 0; i < dim; i++) {
			dst++->id = id;
		}
	}
	if (_options.minval) {
		ssi_size_t id = Factory::AddString("min");
		for (ssi_size_t i = 0; i < dim; i++) {
			dst++->id = id;
		}
	}
	if (_options.maxval) {
		ssi_size_t id = Factory::AddString("max");
		for (ssi_size_t i = 0; i < dim; i++) {
			dst++->id = id;
		}
	}
}

void TupleEventSender::consume (IConsumer::info consume_info,
	ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	ssi_size_t dim = stream_in[0].dim;
	ssi_size_t num = stream_in[0].num;

	ssi_real_t *src = ssi_pcast(ssi_real_t, stream_in[0].ptr);
	ssi_event_tuple_t *dst = ssi_pcast(ssi_event_tuple_t, _event.ptr);
	
	_event.time = ssi_cast (ssi_size_t, 1000 * consume_info.time + 0.5);
	_event.dur = ssi_cast (ssi_size_t, 1000 * consume_info.dur + 0.5);	
	_event.state = consume_info.status == IConsumer::COMPLETED ? SSI_ESTATE_COMPLETED : SSI_ESTATE_CONTINUED;
	
	if (_options.mean) {			
		ssi_real_t *mean = new ssi_real_t[dim];
		ssi_mean(num, dim, src, mean);
		for (ssi_size_t i = 0; i < dim; i++) {
			dst++->value = mean[i];
		}		
	}
	if (_options.var) {
		ssi_real_t *var = new ssi_real_t[dim];
		ssi_var(num, dim, src, var);
		for (ssi_size_t i = 0; i < dim; i++) {
			dst++->value = var[i];
		}
	}
	if (_options.minval || _options.maxval) {
		ssi_real_t *minval = new ssi_real_t[dim];
		ssi_size_t *minpos = new ssi_size_t[dim];
		ssi_real_t *maxval = new ssi_real_t[dim];
		ssi_size_t *maxpos = new ssi_size_t[dim];
		ssi_minmax(num, dim, src, minval, minpos, maxval, maxpos);
		if (_options.minval) {
			for (ssi_size_t i = 0; i < dim; i++) {
				dst++->value = minval[i];
			}
		}
		if (_options.maxval) {
			for (ssi_size_t i = 0; i < dim; i++) {
				dst++->value = maxval[i];
			}
		}
	}

	_listener->update(_event);

}

void TupleEventSender::consume_flush (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	ssi_event_reset (_event);
}



}
