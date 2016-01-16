// MyObject.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2009/09/17
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

#include "MyObject.h"

namespace ssi {

char MyObject::ssi_log_name[] = "myobject__";

MyObject::MyObject(const ssi_char_t *file)
	: _file(0) {

	if (file) {
		if (!OptionList::LoadXML(file, _options)) {
			OptionList::SaveXML(file, _options);
		}
		_file = ssi_strcpy(file);
	}
}

MyObject::~MyObject() {

	if (_file) {
		OptionList::SaveXML(_file, _options);
		delete[] _file;
	}
}

void MyObject::print() {

	ssi_msg(SSI_LOG_LEVEL_BASIC, "calling print()..");
	ssi_print("string=%s\n", _options.string);
	if (!_options.toggle) {
		ssi_wrn("toggle is off");
	}
}

}
