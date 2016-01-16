#include <SDL.h>
#include "graphic/SDL_Window.h"
#include <vector>
#include <mutex>

#ifndef SSI_SDL_WINDOWMANAGER_H
#define SSI_SDL_WINDOWMANAGER_H
namespace ssi {


class SDL_WindowManager//:Thread
{
	public:
	
	static SDL_WindowManager& getInstance()
	{
		static SDL_WindowManager sdl_winm;
		return sdl_winm;
	}
	void run();

	void close();
	void registerWindow(Window* win);
	
	void deregisterWindow(Window* win);


private:

SDL_WindowManager()
{
	init();
}

SDL_WindowManager(SDL_WindowManager const&)  = delete;
void operator=(SDL_WindowManager const&)  = delete;
bool init();
std::vector<Window*> windows;
std::mutex windowsMutex;

};

} // namespace ssi
#endif //SSI_SDL_WINDOWMANAGER_H
