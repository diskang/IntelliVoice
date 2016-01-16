// FileBinaryLZ4.cpp
// author: Andreas Seiderer <seiderer@hcm-lab.de>
// created: 2015/04/06
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

#include "ioput/file/FileBinaryLZ4.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {


bool FileBinaryLZ4::open () {

	if (_is_open) {
		ssi_wrn ("file already open ('%s', %s, %s)", _path, MODE_NAMES[_mode], TYPE_NAMES[_type]);
		return true;
	}

	if (_file) {
		ssi_wrn ("file pointer not empty ('%s', %s, %s)", _path, MODE_NAMES[_mode], TYPE_NAMES[_type]);
		return true;
	}	

	if (!_close_file) {
		ssi_wrn ("no permission to open file ('%s', %s, %s)", _path, MODE_NAMES[_mode], TYPE_NAMES[_type]);
		return false;
	}

	switch (_mode) {
		case File::READ: {
			_file = fopen (_path, "rb");
			break;
		}
		case File::WRITE: {
			_file = fopen (_path, "wb");
			break;
		}
		case File::APPEND: {
			_file = fopen (_path, "ab");
			break;
		}
		case File::READPLUS: {
			_file = fopen (_path, "r+b");
			break;
		}
		case File::WRITEPLUS: {
			_file = fopen (_path, "w+b");
			break;
		}
		case File::APPENDPLUS: {
			_file = fopen (_path, "a+b");
			break;
		}
	}

	if (!_file) {
		ssi_wrn ("fopen() failed (\"%s\")", _path);
		return false;
	}
	
	_is_open = true;

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "opened ('%s', %s, %s)", _path, MODE_NAMES[_mode], TYPE_NAMES[_type]);

	//init LZ4
	LZ4F_errorCode_t errorCode;
	memset(&prefs, 0, sizeof(prefs));

	errorCode = LZ4F_createCompressionContext(&ctx, LZ4F_VERSION);

	prefs.autoFlush = 1;
	prefs.compressionLevel = 16;
	prefs.frameInfo.blockMode = (blockMode_t)1;
	prefs.frameInfo.blockSizeID = (blockSizeID_t)7;
	prefs.frameInfo.contentChecksumFlag = (contentChecksum_t)1;

	firstRun = true;

	return true;
}



bool FileBinaryLZ4::close () {

	if (!_is_open) {
		ssi_wrn ("file already closed ('%s', %s, %s)", _path, MODE_NAMES[_mode], TYPE_NAMES[_type]);
		return true;
	}

	if (!_file) {
		ssi_wrn ("file pointer empty ('%s', %s, %s)", _path, MODE_NAMES[_mode], TYPE_NAMES[_type]);
		return true;
	}

	if (!_close_file) {
		ssi_wrn ("no permission to close file ('%s', %s, %s)", _path, MODE_NAMES[_mode], TYPE_NAMES[_type]);
		return false;
	}

	if (fclose (_file)) {
		ssi_wrn ("fclose() failed ('%s', %s, %s)", _path, MODE_NAMES[_mode], TYPE_NAMES[_type]);
		return false;
	}

	_is_open = false;
	_file = 0;

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "closed ('%s', %s, %s)", _path, MODE_NAMES[_mode], TYPE_NAMES[_type]);

	//clean LZ4
	delete[] in_buff;
	delete[] out_buff;
	LZ4F_freeCompressionContext(ctx);

	return true;
}



bool FileBinaryLZ4::read (void *ptr, ssi_size_t size, ssi_size_t count) {

	ssi_err("reading of LZ4 files not yet implemented!");

	if (!_is_open) {
		ssi_wrn ("file not open (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}

	if (!_read_mode) {
		ssi_wrn ("file not in read mode (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}

	size_t res = fread (ptr, size, count, _file);
	if (res != count) {
		ssi_wrn ("fread() failed (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}

	return true;
}

bool FileBinaryLZ4::write (const void *ptr, ssi_size_t size, ssi_size_t count) {

	if (!_is_open) {
		ssi_wrn ("file not open (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}

	if (!_write_mode) {
		ssi_wrn ("file not in write mode (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}


	if (firstRun) {
		//set buffers on first run with blocksize (in bytes)
		blocksize = size*count;
		in_buff  = new char[blocksize];
		outBuffSize = LZ4F_compressBound(blocksize, &prefs);
		out_buff = new char[outBuffSize];

		int32_t headerSize = LZ4F_compressBegin(ctx, out_buff, outBuffSize, &prefs);
		int32_t sizeCheck = fwrite(out_buff, 1, headerSize, _file);
		
		firstRun = false;
	} else {

		if (blocksize != size*count)
			ssi_err("illegal blocksize change!");
	}

	memcpy(in_buff, ptr, count*size);

	int32_t outSize = LZ4F_compressUpdate(ctx, out_buff, outBuffSize, in_buff, count*size, NULL);
	ssi_size_t res = ssi_cast (ssi_size_t, fwrite (out_buff, 1, outSize, _file));

	ssi_msg (SSI_LOG_LEVEL_BASIC, "LZ4 compr: bytes in: %i | out: %i  | out size: %.2f%%", count*size, outSize, (100.f / (count*size)) * outSize );

	if (res != outSize) {
		ssi_wrn ("fwrite() failed (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}

	return true;
}

bool FileBinaryLZ4::writeLine (const ssi_char_t *string) {

	ssi_err("write line for LZ4 files not yet implemented!");


	if (!_is_open) {
		ssi_wrn ("file not open (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}

	if (!_write_mode) {
		ssi_wrn ("file not in write mode (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}

	bool status;

	ssi_size_t len = ssi_cast (ssi_size_t, strlen (string));
	status = write (&len, sizeof (ssi_size_t), 1);
	if (!status) {
		ssi_err ("write() failed (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
	}
	status = write (string, sizeof (ssi_char_t), len);
	if (!status) {
		ssi_wrn ("write() failed (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}

	return true;
}

bool FileBinaryLZ4::readLine (ssi_size_t num, ssi_char_t *string) {

	ssi_err("read line for LZ4 files not yet implemented!");

	if (!_is_open) {
		ssi_wrn ("file not open (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}

	if (!_read_mode) {
		ssi_wrn ("file not in read mode (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}

	bool status;

	ssi_size_t len;
	status = read (&len, sizeof (ssi_size_t), 1);
	if (!status)
		return false;
	if (num <= len) {
		ssi_err ("input string too short (path=%s, mode=%d, type=%d, shared=false)", _path, _mode, _type);
		return false;
	}
	status = read (string, ssi_cast (ssi_size_t, sizeof (ssi_char_t)), len);
	if (!status)
		return false;
	string[len] = '\0';

	return true;
}


}

