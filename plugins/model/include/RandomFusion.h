// RandomFusion.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2010/06/30
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

#pragma once

#ifndef SSI_MODEL_RANDOMFUSION_H
#define SSI_MODEL_RANDOMFUSION_H

#include "base/IFusion.h"
#include "ioput/file/FileBinary.h"
#include "ioput/option/OptionList.h"

namespace ssi {

class RandomFusion : public IFusion {

public:

	static const ssi_char_t *GetCreateName () { return "ssi_fusion_RandomFusion"; };
	static IObject *Create (const ssi_char_t *file) { return new RandomFusion (); };
	
	IOptions *getOptions () { return 0; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Randomly picks one channel for classification, just for testing purpose."; };

	void setLogLevel (ssi_size_t log_level) {};

	bool train (ssi_size_t n_models,
		IModel **models,
		ISamples &samples);
	bool isTrained () { return _is_trained; };
	bool forward (ssi_size_t n_models,
		IModel **models,
		ssi_size_t n_streams,
		ssi_stream_t **streams,
		ssi_size_t n_probs,
		ssi_real_t *probs);	
	void release ();
	bool save (const ssi_char_t *filepath);
	bool load (const ssi_char_t *filepath);

	ssi_size_t getModelNumber(ISamples &samples){ return samples.getStreamSize (); };

protected:

	RandomFusion ();
	virtual ~RandomFusion ();

	bool _is_trained;

};

}

#endif
