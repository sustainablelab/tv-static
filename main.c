#include <SDL.h>
#include <stdbool.h>
#include "main.h"
#include "window_info.h"

typedef SDL_FPoint AffPoint;

// View polygon artwork
AffPoint view_o = {100, 100};                                   // origin
int view_s = 100;                                               // scale

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
    SDL_Init(SDL_INIT_VIDEO);
    WindowInfo wI; WindowInfo_setup(&wI, argc, argv);           // Init game window info
    win = SDL_CreateWindow(argv[0], wI.x, wI.y, wI.w, wI.h, wI.flags);
    ren = SDL_CreateRenderer(win, -1, 0);
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);       // Draw with alpha

    // Game state
    bool quit = false;
    // Game loop
    while(  quit == false  )
    {
        // Update game state
        // Some game state depends on window size
        SDL_GetWindowSize(win, &wI.w, &wI.h);                   // Get new window size

        // Procedurally generated art
        int poly_cnt = 5; AffPoint *poly;                       // Polygon
        {
            poly = malloc(sizeof(AffPoint)*poly_cnt);           // Alloc mem for polygon

            poly[0] = (AffPoint){0, 0};
            poly[1] = (AffPoint){2, 0};
            poly[2] = (AffPoint){2, 4};
            poly[3] = (AffPoint){0, 4};
            poly[4] = poly[0];
        }
        { // map poly from model to view
            for( int i=0; i<poly_cnt; i++ )
            {
                poly[i].x *= view_s;
                poly[i].y *= view_s;
                poly[i].x += view_o.x;
                poly[i].y += view_o.y;
            }
        }

        // UI
        SDL_Keymod kmod = SDL_GetModState();
        { // Filtered (rapid fire keys)
            SDL_PumpEvents();
            const Uint8 *k = SDL_GetKeyboardState(NULL);        // Get all keys
            if(  kmod&KMOD_SHIFT  )
            {
                if(  k[SDL_SCANCODE_UP]  ) {view_s++; if(view_s>500) {view_s=500;}}
                if(  k[SDL_SCANCODE_DOWN]  ) {view_s--; if(view_s<1) {view_s=1;}}
            }
            else
            {
                if(  k[SDL_SCANCODE_UP]  ) {view_o.y-=2; if(view_o.y<0) {view_o.y=0;}}
                if(  k[SDL_SCANCODE_DOWN]  ) {view_o.y+=2; if(view_o.y>wI.h) {view_o.y=wI.h;}}
                if(  k[SDL_SCANCODE_LEFT]  ) {view_o.x-=2; if(view_o.x<0) {view_o.x=0;}}
                if(  k[SDL_SCANCODE_RIGHT]  ) {view_o.x+=2; if(view_o.x>wI.w) {view_o.x=wI.w;}}
            }
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
        { // Draw Polygon
            SDL_SetRenderDrawColor(ren, 255, 100, 10, 255);      // Alpha doesn't matter here
            SDL_RenderDrawLinesF(ren, poly, poly_cnt);
            free(poly);                                         // Free mem for polygon
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

