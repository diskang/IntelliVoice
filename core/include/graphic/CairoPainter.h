#ifndef SSI_CAIRO_PAITNER_H
#define SSI_CAIRO_PAITNER_H

#include "base/ICanvas.h"
#include "base/IPainter.h"
#include "base/IWindow.h"
#include "base/IColormap.h"

#include <cairo.h>

namespace ssi {

class Painter : public IPainter {


public:

	class Tool : public ITool {

	public:

		static const ssi_rgb_t BLACK;
		static const ssi_rgb_t RED;
		static const ssi_rgb_t GREEN;
		static const ssi_rgb_t BLUE;
		static const ssi_rgb_t WHITE;

		~Tool();

		ssi_handle_t getHandle();
		ssi_rgb_t getColor();
		uint32_t getWidth();
		LINE_STYLES::List getStyle();

	protected:

		ssi_handle_t _handle;
		ssi_rgb_t _color;
		LINE_STYLES::List _style ;
		uint32_t _width;
		
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

	void blank();
	void fill(ITool &brush, ssi_rect_t rect);
	void fill(ITool &brush, ssi_rectf_t rect, bool relative);

	void pixel(ITool &pen, ssi_point_t point);
	void pixel(ITool &pen, ssi_pointf_t point, bool relative);

	void rect(ITool &pen, ITool &brush, ssi_rect_t rect);
	void rect(ITool &pen, ITool &brush, ssi_rectf_t rect, bool relative);
	void line(ITool &pen, ssi_point_t from, ssi_point_t to);
	void line(ITool &pen, ssi_pointf_t from, ssi_pointf_t to, bool relative);
	void ellipse(ITool &pen, ITool &brush, ssi_rect_t rect);
	void ellipse(ITool &pen, ITool &brush, ssi_rectf_t rect, bool relative);

	void text(ITool &font, ITool &pen, ITool &brush, ssi_point_t position, const ssi_char_t *text, TEXT_ALIGN_HORZ::List align_horz = TEXT_ALIGN_HORZ::LEFT, TEXT_ALIGN_VERT::List align_vert = TEXT_ALIGN_VERT::BOTTOM);
	void text(ITool &font, ITool &pen, ITool &brush, ssi_pointf_t position, const ssi_char_t *text, bool relative, TEXT_ALIGN_HORZ::List align_horz = TEXT_ALIGN_HORZ::LEFT, TEXT_ALIGN_VERT::List align_vert = TEXT_ALIGN_VERT::BOTTOM);

	void setBackground(ssi_rgb_t color);

	void setArea(ssi_rect_t);

	ssi_rect_t getArea();

	void end();

protected:

	ssi_rect_t rectf2rect(ssi_rectf_t rect);
	ssi_rect_t rectf2rect(ssi_rectf_t rect, ssi_rect_t area);
	ssi_point_t pointf2point(ssi_pointf_t point);
	ssi_point_t pointf2point(ssi_pointf_t point, ssi_rect_t area);
	
	void cairo_use_pen(ITool &pen);
	void cairo_use_brush(ITool &brush);
	
	double* dashes;
	ssi_rect_t area;
	

	cairo_t* cr;
};

}

#endif //CAIRO_PAINTER_H
