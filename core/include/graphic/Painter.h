// Painter.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2015/01/06
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

#ifndef SSI_GRAPHIC_PAINTER_H
#define SSI_GRAPHIC_PAINTER_H

#include "base/IPainter.h"

namespace ssi {

class Painter : public IPainter {

public:

public:

	class Tool : public ITool {

	public:

		~Tool();

		ssi_handle_t getHandle();
		ssi_rgb_t getColor();

	protected:

		ssi_handle_t _handle;
		ssi_rgb_t _color;
	};

	class Pen : public Tool {

	public:

		Pen(ssi_rgb_t color, WIDTH width = 1, LINE_STYLES::List style = LINE_STYLES::SOLID);
	};

	class Brush : public Tool {

	public:
		
		Brush(); // hollow brush
		Brush(ssi_rgb_t color); 		
	};

	class Font : public Tool {

	public:

		Font(const ssi_char_t *name, WIDTH size = 10, FONT_STYLE font_style = FONT_STYLES::NORMAL);
	};

public:

	Painter();
	virtual ~Painter();

	void begin(ssi_handle_t surface, ssi_rect_t area);

	void setArea(ssi_rect_t area);
	ssi_rect_t getArea();

	void setBackground(ssi_rgb_t color);
	void blank();
	
	void pixel(ITool &pen, ssi_point_t point);
	void pixel(ITool &pen, ssi_pointf_t point, bool relative);

	void fill(ITool &brush, ssi_rect_t rect);
	void fill(ITool &brush, ssi_rectf_t rect, bool relative);

	void rect(ITool &pen, ITool &brush, ssi_rect_t rect);
	void rect(ITool &pen, ITool &brush, ssi_rectf_t rect, bool relative);

	void line(ITool &pen, ssi_point_t from, ssi_point_t to);
	void line(ITool &pen, ssi_pointf_t from, ssi_pointf_t to, bool relative);

	void ellipse(ITool &pen, ITool &brush, ssi_rect_t rect);
	void ellipse(ITool &pen, ITool &brush, ssi_rectf_t rect, bool relative);

	void text(ITool &font, ITool &pen, ITool &brush, ssi_point_t position, const ssi_char_t *text, TEXT_ALIGN_HORZ::List align_horz = TEXT_ALIGN_HORZ::LEFT, TEXT_ALIGN_VERT::List align_vert = TEXT_ALIGN_VERT::BOTTOM);
	void text(ITool &font, ITool &pen, ITool &brush, ssi_pointf_t position, const ssi_char_t *text, bool relative, TEXT_ALIGN_HORZ::List align_horz = TEXT_ALIGN_HORZ::LEFT, TEXT_ALIGN_VERT::List align_vert = TEXT_ALIGN_VERT::BOTTOM);

	void end();

protected:

	ssi_rect_t rectf2rect(ssi_rectf_t rect);
	ssi_rect_t rectf2rect(ssi_rectf_t rect, ssi_rect_t area);
	ssi_point_t pointf2point(ssi_pointf_t point);
	ssi_point_t pointf2point(ssi_pointf_t point, ssi_rect_t area);

	ssi_handle_t _tmp_object;
	ssi_rgb_t _tmp_font_color;
	int _tmp_bk_mode;
	ssi_rgb_t _tmp_bk_color;
	unsigned int _tmp_text_align;

	ssi_handle_t _device;
	ssi_rect_t _area;

	IPainter::ITool *_back_pen;
	IPainter::ITool *_back_brush;
};

}

#endif
