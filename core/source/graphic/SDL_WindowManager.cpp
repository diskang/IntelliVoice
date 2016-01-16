#include "graphic/SDL_WindowManager.h"
#include "thread/Thread.h"
#include "SSI_Cons.h"
#include "graphic/SDL_Window.h"




namespace ssi{

void SDL_WindowManager::run()
	{
		//Start up SDL and create window
		if( !init() )
		{
			printf( "Failed to initialize!\n" );
		}
		else
		{
			
			for(int i=0; i< windows.size(); i++)
			{
				if(windows.at(i)->isShown()==true)
				{
					//windows.at(i)->create();
					
				}
				
			}
			
			int numWin=windows.size();
			//Main loop flag
			bool quit = false;
			bool allWindowsClosed=false;
			bool update=true;
			

			//Event handler
			SDL_Event e;

			//While application is running
			while( !quit )
			{
				//printf("%d\n",numWin);
				if(numWin < windows.size())
				{
					for(int i=numWin; i< windows.size(); i++)
					{
						if(windows.at(i)->isShown()==true)
						{
							
							//windows.at(i)->create();
							update=true;
							windows.at(i)->update();
						}
					}
								
				numWin=windows.size();
				}
				//Handle events on queue
				
				bool isInit=true;
				for(int i=0; i< windows.size(); i++)
					{
						isInit=isInit&&windows.at(i)->isInit();

					}
				if(isInit)
				{
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle window events
					for(int i=0; i< windows.size(); i++)
						windows.at(i)->handleEvent( e );

				}
				}


				//Update all windows
				if(update)
				{
					for(int i=0; i< windows.size(); i++)
					{
						windows.at(i)->update();
					}
				update=false;
				}
				//Check all windows
				allWindowsClosed=true;
				for(int i=0; i< windows.size(); i++)
				{
					if(!windows.at(i)->isClosed())
					{
						allWindowsClosed=false;
					
					}
				}
				//Application closed all windows
				if( allWindowsClosed )
				{
					quit = true;
				}

			}
		}

		//Free resources and close SDL
		close();
	}
	///adds a window to the windowmanager
	void SDL_WindowManager::registerWindow(Window* win)
	{
		windowsMutex.lock();
		windows.push_back(win);
	
		windowsMutex.unlock();
		
	}
	///removes a window from the windowmanager
	void SDL_WindowManager::deregisterWindow(Window* win)
	{
		windowsMutex.lock();
		for(int i=0; i < windows.size(); i++)
		{
			if(windows.at(i)==win)
			{
				windows.erase(windows.begin()+i);
			
			}
		}
		windowsMutex.unlock();
	}

	bool SDL_WindowManager::init()
	{
		//Initialization flag
		bool success = true;

		//Initialize SDL
		if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
		{
			printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Set texture filtering to linear
			if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
			{
				printf( "Warning: Linear texture filtering not enabled!" );
			}


		}

		return success;
	}

	void SDL_WindowManager:: close()
	{
		//Destroy windows
		
	for(int i=0; i< windows.size(); i++)
	{
		//if(!windows.at(i)->isClosed() )
		//	windows.at(i)->close();
			
		delete windows.at(i);
	}
		//Quit SDL subsystems
		SDL_Quit();
	
	}



}//namespace ssi

