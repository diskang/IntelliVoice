// ssi.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/09/05
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

#ifndef SSI_H
#define	SSI_H

#include "base/Factory.h"
#include "base/Array1D.h"

#include "struct/BinTree.h"
#include "struct/Queue.h"

#include "thread/Thread.h"
#include "thread/ClockThread.h"
#include "thread/Event.h"
#include "thread/Lock.h"
#include "thread/Mutex.h"
#include "thread/Timer.h"
#include "thread/Condition.h"
#include "thread/ThreadPool.h"
#include "thread/RunAsThread.h"

#include "ioput/option/OptionList.h"
#include "ioput/option/CmdArgOption.h"
#include "ioput/option/CmdArgParser.h"
#include "ioput/file/File.h"
#include "ioput/file/FileAscii.h"
#include "ioput/file/FileMem.h"
#include "ioput/file/FileMemAscii.h"
#include "ioput/file/FileBinary.h"
#include "ioput/file/FileTools.h"
#include "ioput/file/FilePath.h"
#include "ioput/file/StringList.h"
#include "ioput/file/FileAnnotationWriter.h"
#include "ioput/file/FileProvider.h"
#include "ioput/file/FileStreamIn.h"
#include "ioput/file/FileStreamOut.h"
#include "ioput/file/FileSamplesIn.h"
#include "ioput/file/FileSamplesOut.h"
#include "ioput/file/FileEventsIn.h"
#include "ioput/file/FileEventsOut.h"
#include "ioput/file/FileMessage.h"
#include "ioput/file/FileCSV.h"
#include "ioput/socket/Socket.h"
#include "ioput/socket/SocketUdp.h"
#include "ioput/socket/SocketTcp.h"
#include "ioput/socket/SocketOsc.h"
#include "ioput/socket/SocketOscListener.h"
#include "ioput/socket/SocketOscEventWriter.h"
#include "ioput/socket/SocketImage.h"
#include "ioput/socket/SocketMessage.h"
#include "ioput/pipe/NamedPipe.h"
#include "ioput/wav/WavTools.h"
#include "ioput/example/Example.h"
#include "ioput/example/Exsemble.h"

#include "buffer/Buffer.h"
#include "buffer/TimeBuffer.h"



#include "model/SampleList.h"
#include "model/ModelTools.h"
#include "model/Annotation.h"

#include "signal/SignalTools.h"

#include "event/EventAddress.h"
#include "event/EventList.h"

#include "frame/include/ssiframe.h"
#include "event/include/ssievent.h"

#include "ioput/include/ssiioput.h"
#include "mouse/include/ssimouse.h"

#if _WIN32||_WIN64
#include "graphic/Slider.h"
#include "graphic/Canvas.h"
#include "graphic/PaintData.h"
#include "graphic/Colormap.h"
#include "graphic/GraphicTools.h"
#include "graphic/Monitor.h"
#include "graphic/include/ssigraphic.h"
#else
#include "graphic/SDL_Canvas.h"
#include "graphic/CairoPainter.h"
#include "graphic/PaintData.h"
#include "graphic/Colormap.h"
#include "graphic/GraphicTools.h"
#include "graphic/include/ssigraphic.h"
#endif

#endif
