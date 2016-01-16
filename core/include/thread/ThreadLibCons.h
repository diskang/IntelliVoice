// ThreadLibCons.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/05/21
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

#ifndef SSI_THREAD_THREADLIBCONS_H
#define	SSI_THREAD_THREADLIBCONS_H

#if __MINGW32__||__GNUC__
    #define hasCXX11threads 1

#else
    #define hasCXX11threads 0
#endif // __MINGW32__
// link libraries
#ifdef _MSC_VER
#pragma comment (lib, "Winmm.lib")
#endif

#include "SSI_Cons.h"

#endif


