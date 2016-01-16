// Mutex.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/12
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



#ifndef SSI_THREAD_MUTEX_H
#define SSI_THREAD_MUTEX_H

#include "thread/ThreadLibCons.h"

#if hasCXX11threads
    #include <mutex>
#endif // hasCXX11threads


namespace ssi {

//! \brief A mutex class.
class Mutex {

public:

	//! \brief Constructor
	//
	Mutex ();

	//! \brief Deconstructor
	//
	~Mutex ();

	//! \brief Acquire mutex
	//
	void acquire ();
	//int acquire_try ();

	#if hasCXX11threads
            //special construct to keep interface and behavior of windows implementation
          std::unique_lock<std::mutex> cond_var_unique_lock();
            //returns a unique lock on the mutex
          std::unique_lock<std::mutex> unique_lock();


        #endif // hasCXX11threads

	//! \brief Release mutex
	//
	void release ();

private:

	// the mutex


    #if hasCXX11threads
	  std::mutex critSec;
    #else
      CRITICAL_SECTION critSec;
    #endif // hasCXX11threads

};

}

#endif // _MUTEX_H
