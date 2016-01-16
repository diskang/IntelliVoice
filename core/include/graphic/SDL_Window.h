#include <SDL.h>
#include <mutex>
#include "base/IWindow.h"

#ifndef SSI_SDL_WINDOW_H
#define SSI_SDL_WINDOW_H

namespace ssi{
	
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

#define ssi_int_t int
#define ssi_char_t char

struct Surface : ssi::ISurface
{
	std::mutex* renderMutex;
	SDL_Surface* surface;
};



class Window : public IWindow
{
public:
virtual ~Window();

Window();



void handleEvent( SDL_Event& e );


void focus();


void free();

int getWidth();

int getHeight();

bool hasMouseFocus();


bool hasKeyboardFocus();


bool isMinimized();


bool isShown();

bool isInit();

void show();
void update();
void create();

void setTitel();

void setPosition(ssi_rect_t rect, bool paramsDescribeClientRect=true);

void setTitle(const ssi_char_t *str);

void setClient(IWindowClient *client);

void setStyle(ssi::IWindow::STYLE style);

void close();

bool isClosed()
{
	return mClosed;
}

 
		//Window data
		SDL_Window* mWindow;
		SDL_Renderer* mRenderer;
		int mWindowID;

		//Window dimensions
		int mWidth;
		int mHeight;
		
		//Window position
		int posX;
		int posY;
		bool bUpdate;
		
		//Window focus
		bool mMouseFocus;
		bool mKeyboardFocus;
		bool mFullScreen;
		bool mMinimized;
		bool mShown;
		bool mClosed;
		
		//title
		const char* title;
		
		// ssi window client
		IWindowClient *client;

	
	
};

} //namespace ssi

#endif //SSI_SDL_WINDOW_H
