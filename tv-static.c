#include <SDL.h>
#include <stdbool.h>
#include "main.h"
#include "window_info.h"
#include "rand.h"

void shutdown()
{
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    for(int i=0; i<argc; i++) {puts(argv[i]);}

    // Setup
    rand_init();                                                // seed rand()
    SDL_Init(SDL_INIT_VIDEO);
    WindowInfo wI; WindowInfo_setup(&wI, argc, argv);           // Init game window info
    win = SDL_CreateWindow(argv[0], wI.x, wI.y, wI.w, wI.h, wI.flags);
    ren = SDL_CreateRenderer(win, -1, 0);
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);       // Draw with alpha

    // Game state
    bool quit = false;
    int tv_max = 255;                                           // TV alpha max (brightness)
    // Game loop
    while(  quit == false  )
    {
        // Update game state
        // Some game state depends on window size
        SDL_GetWindowSize(win, &wI.w, &wI.h);                   // Get new window size

        // Procedurally generated art
        int count = 5000; SDL_FPoint *tv_noise; int *tv_alpha;// Rand points w rand alpha
        { // Allocate mem for procedural art
            tv_noise = malloc(sizeof(SDL_FPoint)*count);          // Point locations
            tv_alpha = malloc(sizeof(int)*count);           // Point alpha transparency
        }
        { // Generate TV Static
            for(int i=0; i<count; i++)
            {
                tv_noise[i] = (SDL_FPoint){wI.w/2 + rand_pm(wI.w/2), wI.h/2 + rand_pm(wI.h/2)};
                tv_alpha[i] = rand_0_to_max(tv_max);
            }
        }

        // UI
        { // Filtered (rapid fire keys)
            SDL_PumpEvents();
            const Uint8 *k = SDL_GetKeyboardState(NULL);        // Get all keys
            if(  k[SDL_SCANCODE_UP]  ) {tv_max++; if(tv_max>255) {tv_max=255;}}
            if(  k[SDL_SCANCODE_DOWN]  ) {tv_max--; if(tv_max<0) {tv_max=0;}}
        }
        { // Polled
            SDL_Event e;
            while(  SDL_PollEvent(&e)  )
            {
                if(  e.type == SDL_KEYDOWN  )
                {
                    switch( e.key.keysym.sym)
                    {
                        case SDLK_ESCAPE: quit = true; break;
                        default: break;
                    }
                }
            }
        }

        // Render
        { // Grey Bgnd
            SDL_SetRenderDrawColor(ren, 10, 10, 10, 0);          // Alpha doesn't matter here
            SDL_RenderClear(ren);
        }
        { // Draw the TV Static
            for(int i=0; i<count; i++)
            {
                SDL_SetRenderDrawColor(ren, 255, 255, 255, tv_alpha[i]);
                SDL_RenderDrawPointF(ren, tv_noise[i].x, tv_noise[i].y);
            }
        }
        { // Free mem for old proc art
            free(tv_noise);
            free(tv_alpha);
        }
        { // Display to screen
            SDL_RenderPresent(ren);
            SDL_Delay(10);
        }
    }

    // Shutdown
    shutdown();
    return EXIT_SUCCESS;
}

