#include "graphic/CairoPainter.h"

namespace ssi {
	
Painter::Painter()
{
    dashes=0;
}
Painter::~Painter()
{
	
}
void Painter::fill(ITool &brush, ssi_rect_t rect)
{
	uint32_t color=brush.getColor();
	double r;
	double g;
	double b;
	
	r=(color&0x000000FF)*1.0/256.0;
	g=((color&0x0000FF00)>>8)*1.0/256.0;
	b=((color&0x00FF0000)>>16)*1.0/256.0;
	
	cairo_set_source_rgba(cr,
							r,
							g,
							b,
							1.0);
	cairo_fill_preserve (cr);
	cairo_stroke (cr);
}
void Painter::fill(ITool &brush, ssi_rectf_t rect, bool relative)
{
	cairo_move_to(cr, rect.left, rect.top);
	cairo_line_to(cr, rect.width, rect.top);
	cairo_line_to(cr, rect.width, rect.height);
	cairo_line_to(cr, rect.left, rect.height);
	cairo_close_path(cr);

	cairo_use_brush(brush);
}

void Painter::begin(ssi_handle_t surface, ssi_rect_t area)
{
	cr=(cairo_t*)surface;
	//todo cairo regions?
	//cairo_scale(cr, area.width, area.height);
	this->area=area;
}

void Painter::setBackground(ssi_rgb_t color)
{
	double r;
	double g;
	double b;
	
	r=(color&0x000000FF)*1.0/256.0;
	g=((color&0x0000FF00)>>8)*1.0/256.0;
	b=((color&0x00FF0000)>>16)*1.0/256.0;
	
	cairo_set_source_rgba(cr,
							r,
							g,
							b,
							1.0);
	cairo_fill_preserve (cr);
	cairo_stroke (cr);
}

void Painter::setArea(ssi_rect_t)
{
	this->area=area;
}

ssi_rect_t Painter::getArea()
{
	return area;
}

void Painter::blank()
{
	// Set surface to translucent color (r, g, b, a) without disturbing graphics state. 
	cairo_save (cr);
	cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint (cr);
	cairo_restore (cr);
}

void Painter::pixel(ITool &pen, ssi_point_t point)
{
	cairo_move_to(cr, point.x, point.y);
	
	cairo_set_line_cap  (cr, CAIRO_LINE_CAP_SQUARE);
	cairo_set_line_width (cr, 1.0);
	cairo_rel_line_to(cr, 0.0, 1.0);
	cairo_use_pen(pen);
}
void Painter::pixel(ITool &pen, ssi_pointf_t point, bool relative)
{
	if(relative) cairo_rel_move_to(cr, point.x, point.y);
	else cairo_move_to(cr, point.x, point.y);

	cairo_set_line_cap  (cr, CAIRO_LINE_CAP_SQUARE);
	cairo_set_line_width (cr, 1.0);
	cairo_rel_line_to(cr, 0.0, 1.0);
	cairo_use_pen(pen);
}

void Painter::rect(ITool &pen, ITool &brush, ssi_rect_t rect)
{
	
	cairo_move_to(cr, rect.left, rect.top);
	cairo_line_to(cr, rect.width, rect.top);
	cairo_line_to(cr, rect.width, rect.height);
	cairo_line_to(cr, rect.left, rect.height);
	cairo_close_path(cr);

	cairo_use_brush(brush);


	cairo_use_pen(pen);

}
void Painter::rect(ITool &pen, ITool &brush, ssi_rectf_t rect, bool relative)
{
	if(relative)
	{
		
		
		cairo_rel_move_to(cr, rect.left, rect.top);
		cairo_rel_line_to(cr, rect.width, rect.top);
		cairo_rel_line_to(cr, rect.width, rect.height);
		cairo_rel_line_to(cr, rect.left, rect.height);
		
		cairo_close_path(cr);
		cairo_use_brush(brush);
		
		
		cairo_use_pen(pen);
	}else
	{
		cairo_use_pen(pen);
		cairo_move_to(cr, rect.left, rect.top);
		cairo_line_to(cr, rect.width, rect.top);
		cairo_line_to(cr, rect.width, rect.height);
		cairo_line_to(cr, rect.left, rect.height);
		
		cairo_close_path(cr);
		cairo_use_brush(brush);
		
		
		cairo_use_pen(pen);
	}
}

void Painter::line(ITool &pen, ssi_point_t from, ssi_point_t to)
{
	
	cairo_move_to(cr,from.x*1.0, from.y*1.0);
	cairo_line_to(cr,to.x*1.0, to.y*1.0);
	cairo_use_pen(pen);
}
void Painter::line(ITool &pen, ssi_pointf_t from, ssi_pointf_t to, bool relative)
{
	if(relative)
	{
		
		cairo_rel_move_to(cr,from.x*1.0, from.y*1.0);
		cairo_rel_line_to(cr,to.x*1.0, to.y*1.0);
		cairo_use_pen(pen);
	}
	else
	{
		
		cairo_move_to(cr,from.x*1.0, from.y*1.0);
		cairo_line_to(cr,to.x*1.0, to.y*1.0);
		cairo_use_pen(pen);
	}
}

void Painter::ellipse(ITool &pen, ITool &brush, ssi_rect_t rect)
{
  int xc, yc;
  xc = (rect.width - rect.left)/ 2;
  yc = (rect.height -rect.top) / 2;
  // and finally an ellipse
  double ex, ey, ew, eh;
  // center of ellipse
  ex = xc;
  ey =  (rect.height -rect.top );
  // ellipse dimensions
  ew =  (rect.width - rect.left);
  eh = (rect.height -rect.top );

	
  cairo_save(cr);

  cairo_translate(cr,(rect.left*1.0 + rect.width)/2.0  , (rect.top*1.0 + rect.height)/2.0);  // make (ex, ey) == (0, 0)
  cairo_scale(cr, ew / 2.0, eh / 2.0);  // for width: ew / 2.0 == 1.0
					  // for height: eh / 2.0 == 1.0
   
  cairo_arc(cr, 0.0, 0.0, 1.0, 0.0, 2 * M_PI);  // 'circle' centered at (0, 0)
					  // with 'radius' of 1.0
     
   cairo_restore(cr);
     
  cairo_use_brush(brush);
  cairo_use_pen(pen);
  
}
void Painter::ellipse(ITool &pen, ITool &brush, ssi_rectf_t rect, bool relative)
{
	if(relative)
	{
		//cairo arc is only absolute
		cairo_rel_move_to(cr, rect.left, rect.top);
		int xc, yc;
		xc = (rect.width - rect.left)/ 2;
		yc = (rect.height -rect.top) / 2;
		// and finally an ellipse
		double ex, ey, ew, eh;
		// center of ellipse
		ex = xc;
		ey =  (rect.height -rect.top );
		// ellipse dimensions
		ew =  (rect.width - rect.left);
		eh = (rect.height -rect.top );
	
		
		cairo_save(cr);
	
		cairo_translate(cr,(rect.left*1.0 + rect.width)/2.0  , (rect.top*1.0 + rect.height)/2.0);  // make (ex, ey) == (0, 0)
		cairo_scale(cr, ew / 2.0, eh / 2.0);  // for width: ew / 2.0 == 1.0
					// for height: eh / 2.0 == 1.0
		
		cairo_arc(cr, 0.0, 0.0, 1.0, 0.0, 2 * M_PI);  // 'circle' centered at (0, 0)
							// with 'radius' of 1.0
		
		cairo_restore(cr);
		
		cairo_use_brush(brush);
		cairo_use_pen(pen);
		
		
	}
	else
	{
		int xc, yc;
		xc = (rect.width - rect.left)/ 2;
		yc = (rect.height -rect.top) / 2;
		// and finally an ellipse
		double ex, ey, ew, eh;
		// center of ellipse
		ex = xc;
		ey =  (rect.height -rect.top );
		// ellipse dimensions
		ew =  (rect.width - rect.left);
		eh = (rect.height -rect.top );
	
		
		cairo_save(cr);
	
		cairo_translate(cr,(rect.left*1.0 + rect.width)/2.0  , (rect.top*1.0 + rect.height)/2.0);  // make (ex, ey) == (0, 0)
		cairo_scale(cr, ew / 2.0, eh / 2.0);  // for width: ew / 2.0 == 1.0
					// for height: eh / 2.0 == 1.0
		
		cairo_arc(cr, 0.0, 0.0, 1.0, 0.0, 2 * M_PI);  // 'circle' centered at (0, 0)
							// with 'radius' of 1.0
		
		cairo_restore(cr);
		
		cairo_use_brush(brush);
		cairo_use_pen(pen);
		
	}

}
void Painter::text(ITool &font,
						ITool &pen,
						ITool &brush,
						ssi_point_t position,
						const ssi_char_t *text,
						TEXT_ALIGN_HORZ::List align_horz,
						TEXT_ALIGN_VERT::List align_vert
						)
{
	
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	cairo_set_font_size (cr, 52.0);
	uint32_t color=brush.getColor();
	
	double r;
	double g;
	double b;
	
	r=(color&0x000000FF)*1.0/256.0;
	g=((color&0x0000FF00)>>8)*1.0/256.0;
	b=((color&0x00FF0000)>>16)*1.0/256.0;
	
	cairo_set_source_rgba(cr,
							r,
							g,
							b,
							1.0);


	cairo_font_extents (cr, &fe);
	double x;
	double y;
	cairo_text_extents (cr, text, &te);
		//background

	//align center
	
	x = position.x;
	y = position.y;
	
	if(align_horz==TEXT_ALIGN_HORZ::CENTER)
		x-=(te.width/2 + te.x_bearing);
	else if(align_horz==TEXT_ALIGN_HORZ::RIGHT)
		x-=(te.width + te.x_bearing);
	if(align_vert==TEXT_ALIGN_VERT::CENTER)
		y-=(te.height/2 + te.y_bearing);
	else if(align_vert==TEXT_ALIGN_VERT::TOP)
		y-=(te.height + te.y_bearing);
	
	
	cairo_move_to	(cr, x, y);
	cairo_rel_line_to (cr, 0, -te.height);
	cairo_rel_line_to (cr, te.width, 0);
	cairo_rel_line_to (cr, te.x_bearing, -te.y_bearing);
	cairo_close_path (cr);
	cairo_fill (cr);
	cairo_stroke (cr);
	
	cairo_move_to	(cr, x, y);
	
	color=pen.getColor();
	r=(color&0x000000FF)*1.0/256.0;
	g=((color&0x0000FF00)>>8)*1.0/256.0;
	b=((color&0x00FF0000)>>16)*1.0/256.0;
	
	cairo_set_source_rgba(cr,
							r,
							g,
							b,
							1.0);
	
	cairo_show_text(cr, text);
	

}
void Painter::text(ITool &font, ITool &pen, ITool &brush, ssi_pointf_t position, const ssi_char_t *text, bool relative, TEXT_ALIGN_HORZ::List align_horz, TEXT_ALIGN_VERT::List align_vert)
{
	cairo_font_extents_t fe;
	cairo_text_extents_t te;
	cairo_set_font_size (cr, 52.0);
	uint32_t color=brush.getColor();
	
	double r;
	double g;
	double b;
	
	r=(color&0x000000FF)*1.0/256.0;
	g=((color&0x0000FF00)>>8)*1.0/256.0;
	b=((color&0x00FF0000)>>16)*1.0/256.0;
	
	cairo_set_source_rgba(cr,
							r,
							g,
							b,
							1.0);


	cairo_font_extents (cr, &fe);
	double x;
	double y;
	cairo_text_extents (cr, text, &te);
		//background

	//align center
	
	x = position.x;
	y = position.y;
	
	if(align_horz==TEXT_ALIGN_HORZ::CENTER)
		x-=(te.width/2 + te.x_bearing);
	else if(align_horz==TEXT_ALIGN_HORZ::RIGHT)
		x-=(te.width + te.x_bearing);
	if(align_vert==TEXT_ALIGN_VERT::CENTER)
		y-=(te.height/2 + te.y_bearing);
	else if(align_vert==TEXT_ALIGN_VERT::TOP)
		y-=(te.height + te.y_bearing);
	
	if(relative)
	cairo_rel_move_to	(cr, x, y);
	else
	cairo_move_to	(cr, x, y);
	
	cairo_rel_line_to (cr, 0, -te.height);
	cairo_rel_line_to (cr, te.width, 0);
	cairo_rel_line_to (cr, te.x_bearing, -te.y_bearing);
	cairo_close_path (cr);
	cairo_fill (cr);
	cairo_stroke (cr);
	
	if(relative)
	cairo_rel_move_to	(cr, x, y);
	else
	cairo_move_to	(cr, x, y);
	
	color=pen.getColor();
	r=(color&0x000000FF)*1.0/256.0;
	g=((color&0x0000FF00)>>8)*1.0/256.0;
	b=((color&0x00FF0000)>>16)*1.0/256.0;
	
	cairo_set_source_rgba(cr,
							r,
							g,
							b,
							1.0);
	
	cairo_show_text(cr, text);
	
}
void Painter::cairo_use_pen(ITool &pen)
{
	if(dashes) delete[] dashes;
	dashes=0;

	Tool* toolPtr=(Tool*)&pen;
	uint32_t width=toolPtr->getWidth();
	ITool::LINE_STYLES::List style=toolPtr->getStyle();
	cairo_set_line_width (cr, width*1.0);
	uint32_t color=pen.getColor();
	
	double r=(color&0x000000FF)*1.0/256.0;
	double g=((color&0x0000FF00)>>8)*1.0/256.0;
	double b=((color&0x00FF0000)>>16)*1.0/256.0;
	
	cairo_set_source_rgba(cr,
							r,
							g,
							b,
							1.0);
							
	//cairo_set_dash
	if(style==ITool::LINE_STYLES::SOLID)
	{
		//default anyways?
	}
	else if(style==ITool::LINE_STYLES::DASH)
	{
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
		dashes = new  double[2];
		dashes[0]=width*8.0;
		dashes[1]=width*4.0;
		cairo_set_dash(cr, dashes, 2,0.0);
		
	}
	else if(style==ITool::LINE_STYLES::DOT)
	{
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
		dashes = new  double[2];
		dashes[0]=width*8.0;
		dashes[1]=width*4.0;
		cairo_set_dash(cr, dashes, 2,0.0);
	}
	else if(style==ITool::LINE_STYLES::DASHDOT)
	{
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
		dashes = new  double[3];
		dashes[0]=width*8.0;
		dashes[1]=width*4.0;
		dashes[2]=width*4.0;
		cairo_set_dash(cr, dashes, 3,0.0);

	}
	else if(style==ITool::LINE_STYLES::DASTDOTDOT)
	{
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
		dashes = new  double[3];
		dashes[0]=width*4.0;
		dashes[1]=width*8.0;
		dashes[2]=width*4.0;
		cairo_set_dash(cr, dashes, 3,0.0);
	}

cairo_stroke(cr);
}


void Painter::cairo_use_brush(ITool &brush)
{
	if(brush.getColor()==-1)return;//dont fill
	
	uint32_t color=brush.getColor();
	//cairo_save(cr);
	double r=(color&0x000000FF)*1.0/256.0;
	double g=((color&0x0000FF00)>>8)*1.0/256.0;
	double b=((color&0x00FF0000)>>16)*1.0/256.0;
	cairo_set_source_rgba(cr,
							r,
							g,
							b,
							1.0 );
	cairo_fill_preserve(cr);
	//cairo_set_fill_rule 
	//cairo_fill
	//source&mask?

	//cairo_restore(cr);
}

void Painter::end()
{

	if(dashes)delete[] dashes;
	dashes=0;
	cr=NULL;
}

Painter::Pen::Pen(ssi_rgb_t color, WIDTH width, LINE_STYLES::List style) {
	
_style = style;
_width = width;
_color = color;
}

Painter::Brush::Brush() {

	_color = -1;
}

Painter::Brush::Brush(ssi_rgb_t color) {

	_color = color;
	_width=5;
	_style=IPainter::ITool::LINE_STYLES::SOLID;
}
Painter::Tool::~Tool() {

}

ssi_handle_t Painter::Tool::getHandle() {
	return _handle;
}

ssi_rgb_t Painter::Tool::getColor() {
	return _color;
}
uint32_t Painter::Tool::getWidth()
{
	return _width;
}
Painter::Tool::LINE_STYLES::List Painter::Tool::getStyle()
{
	return _style;
}
//CairoPainter::Tool::
Painter::Font::Font(const ssi_char_t *name, WIDTH size , FONT_STYLE font_style ){}

}
