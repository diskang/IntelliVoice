// IConsumer.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/02/26
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

#pragma once

#ifndef SSI_IOBJECT_H
#define SSI_IOBJECT_H

#include "SSI_Cons.h"
#include "base/IEvents.h"
#include "base/IOptions.h"

namespace ssi {

class IObject : public IEventSender, public IEventListener {

public:

	typedef IObject * (*create_fptr_t) (const ssi_char_t *file); 
	virtual ~IObject () {};

	virtual IOptions *getOptions () = 0;
	virtual const ssi_char_t *getName () = 0;
	virtual const ssi_char_t *getInfo () = 0;

	virtual ssi_object_t getType () { return SSI_OBJECT; };
	virtual void setLogLevel (ssi_size_t level) {};

};

}

#endif
