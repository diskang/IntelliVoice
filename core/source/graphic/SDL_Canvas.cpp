#include "graphic/SDL_Canvas.h"

namespace ssi{
	
	
	DoubleSurface::~DoubleSurface()
	{
		cairo_surface_destroy(cairoSurf[0]);
		cairo_surface_destroy(cairoSurf[1]);
		SDL_FreeSurface(sdlSurf[0]);
		SDL_FreeSurface(sdlSurf[1]);
	}
	DoubleSurface::DoubleSurface()
	{
		w=100, h=100;
		sdlSurf[0] = SDL_CreateRGBSurface (
		0, w, h, 32,
		0x00FF0000, /* Rmask */
		0x0000FF00, /* Gmask */
		0x000000FF, /* Bmask */
		0); /* Amask */
		
		cairoSurf[0] = cairo_image_surface_create_for_data (
		(unsigned char*)sdlSurf[0]->pixels,
		CAIRO_FORMAT_RGB24,
		sdlSurf[0]->w,
		sdlSurf[0]->h,
		sdlSurf[0]->pitch);
		
		sdlSurf[1] = SDL_CreateRGBSurface (
		0, w, h, 32,
		0x00FF0000, /* Rmask */
		0x0000FF00, /* Gmask */
		0x000000FF, /* Bmask */
		0); /* Amask */
		
		cairoSurf[1] = cairo_image_surface_create_for_data (
		(unsigned char*)sdlSurf[1]->pixels,
		CAIRO_FORMAT_RGB24,
		sdlSurf[1]->w,
		sdlSurf[1]->h,
		sdlSurf[1]->pitch);
		
	}
	DoubleSurface::DoubleSurface(int w, int h)
	{
		this->w=w;
		this->h=h;
		
		sdlSurf[0] = SDL_CreateRGBSurface (
		0, w, h, 32,
		0x00FF0000, /* Rmask */
		0x0000FF00, /* Gmask */
		0x000000FF, /* Bmask */
		0); /* Amask */
		
		cairoSurf[0] = cairo_image_surface_create_for_data (
		(unsigned char*)sdlSurf[0]->pixels,
		CAIRO_FORMAT_RGB24,
		sdlSurf[0]->w,
		sdlSurf[0]->h,
		sdlSurf[0]->pitch);
		
		sdlSurf[1] = SDL_CreateRGBSurface (
		0, w, h, 32,
		0x00FF0000, /* Rmask */
		0x0000FF00, /* Gmask */
		0x000000FF, /* Bmask */
		0); /* Amask */
		
		cairoSurf[1] = cairo_image_surface_create_for_data (
		(unsigned char*)sdlSurf[1]->pixels,
		CAIRO_FORMAT_RGB24,
		sdlSurf[1]->w,
		sdlSurf[1]->h,
		sdlSurf[1]->pitch);
	}

	void DoubleSurface::flip()
	{
		
		bufferMutex.lock();
		cairo_surface_t* cairo_tmp=cairoSurf[0];
		SDL_Surface*	sdl_tmp=sdlSurf[0];
		
		cairoSurf[0]=cairoSurf[1];
		sdlSurf[0]=sdlSurf[1];
		
		cairoSurf[1]=cairo_tmp;
		sdlSurf[1]=sdl_tmp;
		bufferMutex.unlock();
		printf("flip!\n");
		
	}
	cairo_surface_t* DoubleSurface::getCairo()
	{
		return cairoSurf[1];
	}
	SDL_Surface* DoubleSurface::getSDL()
	{
		return sdlSurf[0];
	}
	Surface* DoubleSurface::getSurface()
	{
		surf.surface=sdlSurf[0];
		surf.renderMutex=&bufferMutex;
		return &surf;
	}
	void DoubleSurface::resize(int w, int h)
	{

		//free old surfaces
		
		cairo_surface_destroy(cairoSurf[0]);
		SDL_FreeSurface(sdlSurf[0]);
		cairo_surface_destroy(cairoSurf[1]);
		SDL_FreeSurface(sdlSurf[1]);
		
		
		this->w=w;
		this->h=h;
		
		sdlSurf[0] = SDL_CreateRGBSurface (
		0, w, h, 32,
		0x00FF0000, /* Rmask */
		0x0000FF00, /* Gmask */
		0x000000FF, /* Bmask */
		0); /* Amask */
		
		cairoSurf[0] = cairo_image_surface_create_for_data (
		(unsigned char*)sdlSurf[0]->pixels,
		CAIRO_FORMAT_RGB24,
		sdlSurf[0]->w,
		sdlSurf[0]->h,
		sdlSurf[0]->pitch);
		
		sdlSurf[1] = SDL_CreateRGBSurface (
		0, w, h, 32,
		0x00FF0000, /* Rmask */
		0x0000FF00, /* Gmask */
		0x000000FF, /* Bmask */
		0); /* Amask */
		
		cairoSurf[1] = cairo_image_surface_create_for_data (
		(unsigned char*)sdlSurf[1]->pixels,
		CAIRO_FORMAT_RGB24,
		sdlSurf[1]->w,
		sdlSurf[1]->h,
		sdlSurf[1]->pitch);
	}


Canvas::Canvas()
	{
		position={0,0,100,100};
		doubleSurface= new DoubleSurface(100, 100);
		cr = cairo_create (doubleSurface->getCairo());
		renderloop=true;
		renderer= new std::thread(&Canvas::paint, this);
	}



void Canvas::paint()
{
	for(;renderloop;)
	{
		renderMutex.lock();
		if(rerender){
			doubleSurface->flip();
			
			
			printf("area: %d %d\n",position.width-position.left, position.height-position.top );
			
			for(int i=0; i< clients.size(); i++)
			{
				clients.at(i)->paint(this, cr, {0,0, position.width-position.left, position.height-position.top});
			}

			rerender=false;
		
	
	
			//todo flip buffer

			cairo_surface_write_to_png (doubleSurface->getCairo(),
													"/tmp/tst.png");
			cairo_destroy(cr);
			
			cr = cairo_create (doubleSurface->getCairo());
			
			
		}
		renderMutex.unlock();
	}
}

Surface* Canvas::getSurface()
{
	return doubleSurface->getSurface();
}

void Canvas::update()
{
		//check_resize
		
		renderMutex.lock();
		rerender=true;
		
		
	
/*
		SDL_RenderClear( mRenderer );
		SDL_Surface* surface2= SDL_GetWindowSurface( mWindow );
		
		renderMutex.lock();
		SDL_BlitSurface( sdlSurf, NULL, surface2, NULL );
		SDL_UpdateWindowSurface( mWindow );
		*/
		
		renderMutex.unlock();
	
}
void Canvas::addClient(ICanvasClient *client)
{
	clients.push_back(client);
}
void Canvas::setColormap(IColormap::COLORMAP::List type)
{
	
}
void Canvas::mouseDown(ssi_point_t position, KEY vkey)
{
		for(int i=0; i< clients.size(); i++)
			{
				clients.at(i)->mouseDown(this, position, vkey);
			}
}
void Canvas::keyDown(KEY key, KEY vkey)
{
		for(int i=0; i< clients.size(); i++)
			{
				clients.at(i)->keyDown(this, key, vkey);
			}
}

void Canvas::setPosition(ssi_rect_t rect)
{
	//do we need to resize?
	printf("setPos: %d %d %d %d\n",rect.top ,rect.height ,rect.left,rect.width );
	if(rect.width-rect.left != position.width -position.left ||
		rect.height-rect.top != position.height-position.top)
		{
			if(rect.height-rect.top<0||rect.width-rect.left<0)return;
			
			
			renderMutex.lock();
			cairo_destroy(cr);
			doubleSurface->resize(rect.width-rect.left, rect.height-rect.top);
			

			cr = cairo_create (doubleSurface->getCairo());
			

			position=rect;
			renderMutex.unlock();
			
		}
}

void Canvas::clear()
{
	renderMutex.lock();
	
	cairo_set_source_rgba (cr, 1.0f, 0.0f, 0.05f, 1.0f);
	cairo_fill_preserve (cr);
	cairo_stroke (cr);
	renderMutex.unlock();
}

void Canvas::setBackground(ssi_rgb_t color)
{
	renderMutex.lock();
	
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
	
	renderMutex.unlock();
}
Canvas::~Canvas()
{
	renderloop=false;
	renderer->join();
	delete doubleSurface;
	cairo_destroy(cr);

	

}

}//namespace ssi
