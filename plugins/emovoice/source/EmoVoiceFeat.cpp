// EmoVoiceFeat.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/10/26
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

#include "EmoVoiceFeat.h"

#include "ioput/wav/WavTools.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

EmoVoiceFeat::EmoVoiceFeat (const ssi_char_t *file)
	: _fex (0),
	_feature_vector (0),
	_fselection (0),
	_file (0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

EmoVoiceFeat::~EmoVoiceFeat () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

ssi_size_t EmoVoiceFeat::getSampleDimensionOut (ssi_size_t sample_dimension_in) {

	if (sample_dimension_in != 1) {
		ssi_err ("sample dimension > 1 not supported");		
	}

	ssi_size_t dim = 0;
	switch (_options.maj) {
		case 1:
			dim = V1_N_FEATURES;
			break;
		case 2:
			dim = V2_N_FEATURES;
			break;
		default:
			ssi_err ("unkown version");
	}		

	return dim;
}

ssi_size_t EmoVoiceFeat::getSampleBytesOut (ssi_size_t sample_bytes_in) {
	return sizeof (mx_real_t);
}

ssi_type_t EmoVoiceFeat::getSampleTypeOut (ssi_type_t sample_type_in) {

	if (sample_type_in != SSI_SHORT && sample_type_in != SSI_FLOAT) {
		ssi_err ("sample type %s not supported", SSI_TYPE_NAMES[sample_type_in]);		
	}

	return SSI_FLOAT;
}


void EmoVoiceFeat::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	if (stream_in.type == SSI_FLOAT) {
		ssi_stream_init (_stream_short, 0, stream_in.dim, sizeof (short), SSI_SHORT, stream_in.sr);
	}

	// init feature extraction
	_fex = fextract_create (0, _options.m_e_params, _options.maj, _options.min);
	_n_features_reduced = _n_features = _fex->n_features;

	//_options.fselect = 1;
	//_options.fselection_def = "10,20,30,40";

	// init feature selection
	if (_options.fselect) {
		_fselection = fx_select_create(_n_features);
		fx_select_sscan(_fselection, _options.fselection_def);
		_n_features_reduced = _fselection->n_selected;
	}

}

void EmoVoiceFeat::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	ssi_size_t sample_number = stream_in.num;

	short *inptr = 0;
	if (stream_in.type == SSI_FLOAT) {
		ssi_stream_adjust (_stream_short, stream_in.num);
		inptr = ssi_pcast (short, _stream_short.ptr);
		float *srcptr = ssi_pcast (float, stream_in.ptr);
		for (ssi_size_t i = 0; i < stream_in.num * stream_in.dim; i++) {
			*inptr++ = ssi_cast (short, *srcptr++ * 32768.0f);
		}
		inptr = ssi_pcast (short, _stream_short.ptr);
	} else {
		inptr = reinterpret_cast<short *> (stream_in.ptr);
	}
	mx_real_t *outptr = reinterpret_cast<mx_real_t *> (stream_out.ptr);

	if (_options.fselect) {
		if (_feature_vector == 0) {
			_feature_vector = new mx_real_t[_n_features];
		}			
	} else {
		_feature_vector = outptr;
	}

	_fex = fextract_soft_reset (_fex, sample_number);
	if (fextract_calc (_fex, _feature_vector, inptr, _options.maj, _options.min) != _n_features) {
		ssi_err ("Feature extraction was not successful!");
	}

	if (_options.fselect) {
		if (fx_select_apply (&outptr, _fselection, _feature_vector) != _n_features) {
			ssi_err ("Feature selection did not succeed!");
		}
	}

}

void EmoVoiceFeat::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	if (_fex) {
		fextract_destroy (_fex);
	}

	if (_fselection) {
		fx_select_destroy (_fselection);
	}

	if (_options.fselect) {
		delete[] _feature_vector;
	}

	if (stream_in.type == SSI_FLOAT) {
		ssi_stream_destroy (_stream_short);
	}

}

}
