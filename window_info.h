#ifndef __WINDOW_INFO_H__
#define __WINDOW_INFO_H__
/* *************Example***************
 * int main(int argc, char *argv[])
 * {
 *      WindowInfo wI; WindowInfo_setup(&wI, argc, argv);
 *      ...
 * }
 * *******************************/
 /* *************Write to WindowInfo struct***************
 * Write direct to WindowInfo struct as an alternative to calling
 * WindowInfo_setup().
 * *******************************/
/* *************Write typical window x,y,w,h***************
 *
 * Use the x,y,w,h args passed from Vim:
 *
 * int main(int argc, char *argv[])
 * {
 *      WindowInfo wI = {.x=50, .y=50, .w=800, .h=600, .flags=0};
 *      if(argc>1) wI.x = atoi(argv[1]);
 *      if(argc>2) wI.y = atoi(argv[2]);
 *      if(argc>3) wI.w = atoi(argv[3]);
 *      if(argc>4) wI.h = atoi(argv[4]);
 *      ...
 * }
 * *******************************/
/* *************Write typical window flags***************
 *
 *      Uint32 flags = SDL_WINDOW_BORDERLESS |
 *                     SDL_WINDOW_ALWAYS_ON_TOP |
 *                     SDL_WINDOW_INPUT_GRABBED;
 *      wI.flags = flags;
 *
 * BORDERLESS       :   looks nice
 *
 * ALWAYS_ON_TOP    :   I pretend window is "inside" Vim by putting it on top of
 *                      a Vim window
 *
 * INPUT_GRABBED    :   my hack to make ALWAYS_ON_TOP work
 *                      without clicking the mouse in the window
 * *******************************/
typedef struct
{
    int x;
    int y;
    int w;
    int h;
    Uint32 flags;
} WindowInfo;

void WindowInfo_setup(WindowInfo *wI, int argc, char *argv[])
{ // Window size, location, and behavior to act like a Vim window
    wI->x = 50;                                                 // Default x
    wI->y = 50;                                                 // Default y
    wI->w = 800;                                                // Default w
    wI->h = 600;                                                // Default h
    if(argc>1) wI->x = atoi(argv[1]);                           // Vim window x
    if(argc>2) wI->y = atoi(argv[2]);                           // Vim window y
    if(argc>3) wI->w = atoi(argv[3]);                           // Vim window w
    if(argc>4) wI->h = atoi(argv[4]);                           // Vim window h
    if(argc>1)
    {
        wI->flags = SDL_WINDOW_BORDERLESS |                     // Look pretty
                    SDL_WINDOW_ALWAYS_ON_TOP |                  // Stay on top
                    SDL_WINDOW_INPUT_GRABBED;                   // Really stay on top
    }
    else
    {
        wI->flags = SDL_WINDOW_RESIZABLE;                       // Click-drag to resize
    }
}

#endif // __WINDOW_INFO_H__
