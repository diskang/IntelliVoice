// XMLEventHelper.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2014/10/27
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

#ifndef SSI_IOPUT_XMLEVENTHELPER_H
#define SSI_IOPUT_XMLEVENTHELPER_H

#include "SSI_Cons.h"
#include "base/IEvents.h"
#include "thread/Thread.h"

namespace ssi {

class TiXmlDocument;
class TiXmlElement;
class TiXmlBase;
class EventAddress;
class Mutex;
class XMLEventSender;
class Monitor;
class Timer;

class XMLEventHelper : public Thread {

public:

	struct MapType {
		enum List {
			UNDEF = 0,
			STREAM = 1,
			EVENT = 2
		};
	};

	struct MapTarget {
		enum List {
			UNDEF = 0,
			ATTRIBUTE = 0,
			ELEMENT
		};
	};

	struct Field {
		enum List {
			VALUE = 0,
			NAME,
			TIME,
			SYSTEMTIME,
			RELATIVETIME,
			DURATION,
			STATE,
			EVENT,
			SENDER
		};
	};

	static const ssi_char_t *FieldName(Field::List field) {

		switch (field) {
			case Field::VALUE: return "value";
			case Field::NAME: return "name";
			case Field::TIME: return "time";
			case Field::SYSTEMTIME: return "systemtime";
			case Field::RELATIVETIME: return "relativetime";
			case Field::DURATION: return "duration";
			case Field::STATE: return "state";
			case Field::EVENT: return "event";
			case Field::SENDER: return "sender";
		}

		return 0;
	}

	struct Mapping {
		MapType::List type;
		MapTarget::List target;
		TiXmlBase *node;
		ssi_size_t stream_id;
		ssi_size_t n_event_ids;
		ssi_size_t *event_ids;
		ssi_size_t n_sender_ids;
		ssi_size_t *sender_ids;
		ssi_size_t n_select;
		ssi_size_t *select;
		ssi_size_t span;
		ssi_size_t last;
		Field::List field;
		bool mean;
	};

public: 	

	XMLEventHelper(XMLEventSender *sender);
	virtual ~XMLEventHelper ();

	void setLogLevel (int level) {
		ssi_log_level = level;
	}

	bool parse();
	bool forward(ssi_size_t n_streams,
		ssi_stream_t streams[],
		ssi_size_t time);
	bool forward(IEvents &events, 
		ssi_size_t n_new_events,
		ssi_size_t time);
	void send(ssi_size_t time, 
		ssi_size_t dur);

	void enter();
	void run();
	void flush();

protected:

	static ssi_char_t *ssi_log_name;
	int ssi_log_level;

	XMLEventSender *_sender;
	
	ssi_size_t _n_strbuf;
	ssi_char_t *_strbuf;
	ssi_char_t _row_delim;
	ssi_char_t _col_delim;
	
	Mutex *_mutex;
	Timer *_timer;
	
	bool parseChildren(TiXmlElement *elem);
	bool parseAttributes(TiXmlElement *elem);
	bool parseMapping(MapTarget::List target, TiXmlBase *node, const ssi_char_t *string);	
	bool parseStream(Mapping &map, const ssi_char_t *string);
	bool applyStream(Mapping &map, ssi_stream_t &stream);
	bool parseEvent(Mapping &map, const ssi_char_t *string);
	bool checkEvent(Mapping &map, ssi_event_t &e);
	bool applyEvent(Mapping &map, ssi_event_t &e);
	bool parseTimeout(Mapping &map, const ssi_char_t *string);
	bool parseSelect(Mapping &map, const ssi_char_t *string);
	bool parseField(Mapping &map, const ssi_char_t *string);	
	bool parseMean(Mapping &map, const ssi_char_t *string);

	std::vector < Mapping > mapping;
	void resetMapping(ssi_size_t time);
	void initMapping(Mapping &map);
	void clearMapping();
	
};

}

#endif

