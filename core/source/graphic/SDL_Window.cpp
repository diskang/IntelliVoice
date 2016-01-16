#include "graphic/SDL_Window.h"
#include "graphic/SDL_WindowManager.h"
#include <stdio.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include <cairo.h>
/*
//sdlmain
	//windowmanager
		//<window*>
		//texture

//threaded surface
	doublebuffer
 

//canvas
	//window
	//threadedsurface
	//render()
*/
namespace ssi{

Window::~Window()
{

}
Window::Window()
{
	//Initialize non-existant window
	mWindow = NULL;
	mRenderer = NULL;

	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mShown = false;
	mWindowID = -1;
	mClosed=false;
	bUpdate=false;
	client=0;
	

	mWidth = SCREEN_WIDTH;
	mHeight = SCREEN_HEIGHT;
	posX=0;
	posY=0;
	title=0;
	
	SDL_WindowManager::getInstance().registerWindow(this);
	//init();
	
}

void Window::create()
{
	
	//Create window
	mWindow = SDL_CreateWindow( "SDL Uninitialized", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	if( mWindow != NULL )
	{
		mMouseFocus = true;
		mKeyboardFocus = true;


		//Create renderer for window
		mRenderer = SDL_CreateRenderer( mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
		if( mRenderer == NULL )
		{
			SDL_WindowManager::getInstance().deregisterWindow(this);
			printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
			SDL_DestroyWindow( mWindow );
			mWindow = NULL;
		}
		else
		{
	
			//Initialize renderer color
			SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

			//Grab window identifier
			mWindowID = SDL_GetWindowID( mWindow );

			//Flag as opened
			mShown = true;
		}
	}
	else
	{
		printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
	}
	
	

	//return mWindow != NULL && mRenderer != NULL;
}

void Window::handleEvent( SDL_Event& e )
{
	//used for ssi mouse and key events
	IWindowClient::KEY key=IWindowClient::VIRTUAL_KEYS::NONE;
	
	//If an event was detected for this window
	if( e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID )
	{


		switch( e.window.event )
		{
			//Window appeared
			case SDL_WINDOWEVENT_SHOWN:
			mShown = true;
			break;

			//Window disappeared
			case SDL_WINDOWEVENT_HIDDEN:
			mShown = false;
			break;

			//Get new dimensions and repaint
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent( mRenderer );
			
			//updates clients position
			if(client)
			{
				int x,y;
				SDL_GetWindowPosition(mWindow, &x,&y);
				client->setPosition(ssi_rect_t
											{
												x,
												y,
												mWidth,
												mHeight
											}
									);
				
			}
			update();
			break;

			//Repaint on expose
			case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent( mRenderer );
			break;

			//Mouse enter
			case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			break;
			
			//Mouse exit
			case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			break;

			//Keyboard focus gained
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			update();

			break;
			
			//Keyboard focus lost
			case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;

			break;

			//Window minimized
			case SDL_WINDOWEVENT_MINIMIZED:
            mMinimized = true;
            break;

			//Window maxized
			case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
			update();
            break;
			
			//Window restored
			case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
			update();
            break;

			//Hide on close
			case SDL_WINDOWEVENT_CLOSE:
			SDL_HideWindow( mWindow );
			close();
			break;
			
			
			}
			


		
		if(bUpdate)
		{
			if(title)SDL_SetWindowTitle( mWindow, title );
			SDL_SetWindowPosition(mWindow,
                           posX,
                           posY);
                           
			SDL_SetWindowSize(mWindow,
                           mWidth,
                           mHeight);
			bUpdate=false;
			update();
		}

	}
	else if( e.type == SDL_KEYDOWN )
		{
			/*
			struct VIRTUAL_KEYS {
			enum List : KEY {
			NONE = 0,
			SHIFT_LEFT = 1 << 0,
			SHIFT_RIGHT = 1 << 1,
			CONTROL_LEFT = 1 << 2,
			CONTROL_RIGHT = 1 << 3,
			ALT_LEFT = 1 << 4,
			ALT_RIGHT = 1 << 5,
			MOUSE_LEFT = 1 << 6,
			MOUSE_MIDDLE = 1 << 7,
			MOUSE_RIGHT = 1 << 8
			*/
			
			
			switch( e.key.keysym.sym )
			{
				case SDLK_LSHIFT: key |= IWindowClient::VIRTUAL_KEYS::SHIFT_LEFT;
				break;

				case SDLK_RSHIFT: key |= IWindowClient::VIRTUAL_KEYS::SHIFT_RIGHT;
				break;

				case SDLK_LCTRL: key |= IWindowClient::VIRTUAL_KEYS::CONTROL_LEFT;
				break;

				case SDLK_RCTRL: key |= IWindowClient::VIRTUAL_KEYS::CONTROL_RIGHT;
				break;

				case SDLK_LALT: key |= IWindowClient::VIRTUAL_KEYS::ALT_LEFT;
				break;

				case SDLK_RALT: key |= IWindowClient::VIRTUAL_KEYS::ALT_RIGHT;
				break;


			}
		}
		else if(e.type == SDL_BUTTON_LEFT)
		{
			key |= IWindowClient::VIRTUAL_KEYS::MOUSE_LEFT;
		}
		else if(e.type == SDL_BUTTON_RIGHT)
		{
			key |= IWindowClient::VIRTUAL_KEYS::MOUSE_RIGHT;
		}
		else if(e.type == SDL_BUTTON_MIDDLE)
		{
			key |= IWindowClient::VIRTUAL_KEYS::MOUSE_MIDDLE;
		}
		
		//give key event to ssi client
		if(client)
		{
			//if(mKeyboardFocus)client->keyDown(key,0);
		}
}
void Window::close()
{
	mClosed=true;
	free();
	
}
void Window::focus()
{
	//Restore window if needed
	if( !mShown )
	{
		SDL_ShowWindow( mWindow );
	}

	//Move window forward
	SDL_RaiseWindow( mWindow );
	update();
}
void Window::update()
{

	if( !mMinimized )
	{	


	
	
	if (client)
	{
		client->update();
		
		SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( mRenderer );
		//SDL_Surface* surface2= SDL_GetWindowSurface( mWindow );
		Surface* surf=(Surface*)client->getSurface();
		
		
		if(surf)
		{
			surf->renderMutex->lock();
			//SDL_BlitSurface( surf->surface, NULL, surface2, NULL );
			SDL_Texture* tx= SDL_CreateTextureFromSurface(mRenderer, surf->surface);
			SDL_RenderCopy( mRenderer, tx, NULL, NULL );

			surf->renderMutex->unlock();
		}
	}
	else
	{
		//do some sdl test rendering
		
		//Clear screen
		SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( mRenderer );
        
		SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
		SDL_SetRenderDrawColor( mRenderer, 0x00, 0xFF, 0x00, 0xFF );
		SDL_RenderFillRect( mRenderer, &fillRect );
	}
	SDL_RenderPresent( mRenderer );
	SDL_UpdateWindowSurface( mWindow );
	}
}
void Window::show()
{
	mShown=true;
}

void Window::setTitle(const ssi_char_t *str)
{
	title=str;
	bUpdate=true;
}
void Window::setPosition(ssi_rect_t rect, bool paramsDescribeClientRect)
{
	mHeight=rect.height;
	mWidth=rect.width;
	posX=rect.left;
	posY=rect.top;
							   
	bUpdate=true;
	
	if(client)
	{
		client->setPosition(rect);
	}
	update();


}
void Window::free()
{
	//remove from windowmanager
	SDL_WindowManager::getInstance().deregisterWindow(this);
	if(client)
	{
		//delete client;
	}
	SDL_DestroyRenderer( mRenderer );
	if( mWindow != NULL )
	{
		SDL_DestroyWindow( mWindow );
	}
	
	
	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
	


}
void Window::setClient(IWindowClient *client)
{
	this->client=client;
}
void Window::setStyle(ssi::IWindow::STYLE style)
{
	
}
int Window::getWidth()
{
	return mWidth;
}

int Window::getHeight()
{
	return mHeight;
}

bool Window::hasMouseFocus()
{
	return mMouseFocus;
}

bool Window::hasKeyboardFocus()
{
	return mKeyboardFocus;
}
bool Window::isInit()
{
	return mWindow!=NULL;
}

bool Window::isMinimized()
{
	return mMinimized;
}

bool Window::isShown()
{
	return mShown;
}

} //namespace ssi
