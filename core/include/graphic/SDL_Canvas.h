#ifndef SSI_SDL_CANVAS_H
#define SSI_SDL_CANVAS_H

#include <thread>
#include <mutex>
#include "graphic/SDL_Window.h"
#include "base/ICanvas.h"
#include <cairo.h>
#include <SDL.h>
#include <vector>

namespace ssi{
	
class DoubleSurface
{
	public:
	
	DoubleSurface();
	DoubleSurface(int w, int h);
	~DoubleSurface();
	void flip();
	cairo_surface_t* getCairo();
	SDL_Surface* getSDL();
	void resize(int w, int h);
	Surface* getSurface();
	
	private:
	
	SDL_Surface* sdlSurf[2];
	cairo_surface_t* cairoSurf[2];
	int w, h;
	std::mutex bufferMutex;
	Surface surf;
};

class Canvas:public ICanvas 
{
	public:
	Canvas();
	~Canvas();

	
	void update();
	void paint();
	void addClient(ICanvasClient *client);
	void setColormap(IColormap::COLORMAP::List type);
	void setBackground(ssi_rgb_t color);
	void setPosition(ssi_rect_t rect);
	void mouseDown(ssi_point_t position, KEY vkey);
	void keyDown(KEY key, KEY vkey);
	void clear();
	Surface* getSurface();
	
	
	private:
	
	ssi_rect_t position;
	Surface* surf;
	bool rerender;
	std::thread* renderer;
	std::mutex renderMutex;
	std::vector<ICanvasClient*> clients;
	DoubleSurface* doubleSurface;
	cairo_t* cr;
	bool renderloop;
	
};

} //namespace ssi

#endif //SSI_SDL_CANVAS_H
