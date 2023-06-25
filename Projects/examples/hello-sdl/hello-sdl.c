#include <iostream>

#include <SDL/SDL.h>


void logSDLVersion(std::ostream &out,
                   const std::string& what,
                   const SDL_version& compiled,
                   const SDL_version& linked,
                   std::string revision)
{
  out << what << " Version (Compiled): "
      << static_cast<int>(compiled.major) << "."
      << static_cast<int>(compiled.minor) << "."
      << static_cast<int>(compiled.patch);
  if (!revision.empty())
    out << " (" << revision << ")";
  out << std::endl;

  out << what << " Version (Runtime):  "
      << static_cast<int>(linked.major) << "."
      << static_cast<int>(linked.minor) << "."
      << static_cast<int>(linked.patch)
      << std::endl;
}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    std::cout << "Failed to initialize SDL" << SDL_GetError() << std::endl;

  std::cout << "Hello SDL!" << std::endl;

  // Display SDL version information
  SDL_version compiled;
  SDL_version linked;
  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);
  logSDLVersion(std::cout, "SDL", compiled, linked, SDL_GetRevision());

  // Create a window and renderer using SDL
  SDL_Window* window = SDL_CreateWindow("SDL Window",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        240, 120,
                                        SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_SetRenderDrawColor(renderer, 20, 40, 60, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  // Wait for user event before closing
  bool isRunning = true;
  SDL_Event event;
  while (isRunning) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN &&
          event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        isRunning = false;
      else if (event.type == SDL_QUIT)
        isRunning = false;
    }
    SDL_Delay(30);
  }

  // Cleanup
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return 0;
}
