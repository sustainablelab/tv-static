#include <SDL.h>
#include <stdbool.h>
#include "main.h"
#include "window_info.h"

typedef SDL_FPoint AffPoint;                                    // point
typedef AffPoint AffVec;                                        // vector

AffVec aff_vec_from_points(AffPoint A, AffPoint B)
{ // Return vector AB (the vector that goes from A to B)
    return (AffPoint){B.x-A.x, B.y-A.y};
}
typedef struct
{
    AffPoint A, B;
} AffSeg;
// Alias affine segments as oriented sides
typedef AffSeg AffOrS;                                          // oriented side
float aff_sarea_poly(AffPoint *poly, int n)
{ // Signed area of polygon with n points
    /* *************DOC***************
     * Uses definition of signed area of a polygon as the sum of
     * the signed areas of each oriented side.
     *
     * The order of the points (clockwise or counter clockwise)
     * affects the sign of the signed area.
     *
     * clockwise            : signed area is positive
     * counter-clockwise    : signed area is negative
     * *******************************/
    float s = 0;                                                // Total signed area
    AffVec u,v;
    for(int i=0; i<(n-1); i++)
    {
        u = poly[i]; v = poly[i+1];
        s += 0.5*(u.x*v.y - v.x*u.y);
    }
    return s;
}

typedef struct
{
    float a,b,c;
} AffLine;                                                      // line (infinite extent)
AffLine aff_join_of_points(AffPoint A, AffPoint B)
{ // Return join of points A and B
    float alpha = B.x-A.x; float beta = B.y-A.y;
    float c = -1*beta*A.x + alpha*A.y;
    AffLine l = {-1*beta, alpha, c};
    return l;
}
AffPoint aff_meet_of_lines(AffLine l1, AffLine l2)
{ // Return meet of lines l1 and l2 <--? What happens if lines don't intersect?
    /* *************DOC***************
     * TODO:
     * - Return meet by passing meet as a pointer arg
     * - Use return value for a success/fail (meet/no-meet)
     * *******************************/
    float a1 = l1.a; float b1 = l1.b; float c1 = l1.c;
    float a2 = l2.a; float b2 = l2.b; float c2 = l2.c;
    float det = 1/(a1*b2 - a2*b1);                              // What happens when 1/0?
    float x = det*(b2*c1 - b1*c2);
    float y = det*(a1*c2 - a2*c1);
    AffPoint M = {x,y};
    return M;
}


// View polygon artwork
AffPoint view_o = {200, 0};                                   // origin
int view_s = 122;                                             // scale
// DEBUG by moving scanline manually
int Y = 0;                                                    // scanline y set by UI

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
        int poly_cnt = 9; AffPoint *poly;                       // Polygon
        {
            poly = malloc(sizeof(AffPoint)*poly_cnt);           // Alloc mem for polygon

            poly[0] = (AffPoint){0, 1};
            poly[1] = (AffPoint){2, 0};
            poly[2] = (AffPoint){1, 1.5};
            poly[3] = (AffPoint){2, 2.5};
            poly[4] = (AffPoint){3, 2.5};
            poly[5] = (AffPoint){2, 4};
            poly[6] = (AffPoint){0, 5};
            poly[7] = (AffPoint){-1, 2};
            poly[8] = poly[0];
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
        AffPoint topmost, botmost;
        { // fill the polygon
            { // find the top-most and bottom-most vertex
                topmost = poly[0]; botmost = poly[0];
                for( int i=0; i<poly_cnt; i++ )
                {
                    if(  topmost.y > poly[i].y  ) { topmost = poly[i]; }
                    if(  botmost.y < poly[i].y  ) { botmost = poly[i]; }
                }
            }
        }

        // UI
        SDL_Keymod kmod = SDL_GetModState();
        { // Filtered (rapid fire keys)
            SDL_PumpEvents();
            const Uint8 *k = SDL_GetKeyboardState(NULL);        // Get all keys
            if(  !(kmod&KMOD_CTRL)  ) // block these controls if CTRL is held down
            {
                if(  kmod&KMOD_SHIFT  )
                {
                    /* if(  k[SDL_SCANCODE_UP]  ) {view_s++; if(view_s>500) {view_s=500;}} */
                    /* if(  k[SDL_SCANCODE_DOWN]  ) {view_s--; if(view_s<1) {view_s=1;}} */
                }
                else
                {
                    if(  k[SDL_SCANCODE_UP]  ) {view_o.y-=2; if(view_o.y<0) {view_o.y=0;}}
                    if(  k[SDL_SCANCODE_DOWN]  ) {view_o.y+=2; if(view_o.y>wI.h) {view_o.y=wI.h;}}
                    if(  k[SDL_SCANCODE_LEFT]  ) {view_o.x-=2; if(view_o.x<0) {view_o.x=0;}}
                    if(  k[SDL_SCANCODE_RIGHT]  ) {view_o.x+=2; if(view_o.x>wI.w) {view_o.x=wI.w;}}
                }
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
                        case SDLK_UP:
                              if(  kmod&KMOD_CTRL  )
                              { Y--; if(Y<topmost.y) {Y=topmost.y;} }
                              else if(  kmod&KMOD_SHIFT  )
                              {view_s++; if(view_s>500) {view_s=500;}}
                              break;
                        case SDLK_DOWN:
                              if(  kmod&KMOD_CTRL  )
                              { Y++; if(Y>botmost.y) {Y=botmost.y;} }
                              else if(  kmod&KMOD_SHIFT  )
                              {view_s--; if(view_s<1) {view_s=1;}}
                              break;
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
            /* free(poly);                                         // Free mem for polygon */
        }
        { // Highlight top-most point
            SDL_SetRenderDrawColor(ren, 255, 0, 0, 200);
            int s = 4;
            SDL_FRect highlight = {.x=topmost.x-s, .y=topmost.y-s, .w=2*s, .h=2*s};
            SDL_RenderDrawRectF(ren, &highlight);
        }
        { // Highlight bottom-most point
            SDL_SetRenderDrawColor(ren, 10, 100, 255, 200);
            int s = 4;
            SDL_FRect highlight = {.x=botmost.x-s, .y=botmost.y-s, .w=s*2, .h=s*2};
            SDL_RenderDrawRectF(ren, &highlight);
        }
        if(1) // scanline : fill polygon (TODO: find intersections)
        { // Fill the polygon
            /* int y=topmost.y;                                    // Scan-line method */
            float y=topmost.y;                                    // Scan-line method
            SDL_SetRenderDrawColor(ren, 200, 100, 10, 180);     // Set fill color
            // Make a list of lines out of the polygon sides
            AffLine sides[poly_cnt-1];
            for( int i=0; i<(poly_cnt-1); i++ )
            {
                sides[i] = aff_join_of_points(poly[i], poly[i+1]);
            }
            while(  y<botmost.y  )
            {
                AffLine scanline = {0, 1, y};                   // Line : y = constant
                // Find intersection of scanline with each side
                int meet_cnt = 0;                               // Count intersections
                AffPoint meets[poly_cnt];                       // At most 1 meet per poly seg
                for( int i=0; i<(poly_cnt-1); i++ )
                {
                    AffPoint meet = aff_meet_of_lines(scanline, sides[i]);
                    // DEBUG: does this fix bugs where fill line is dropped?
                    // Nope, makes it worse!
                    /* meet.x = (int)meet.x; meet.y = (int)meet.y; */

                    // Draw the portions of the scan line that are inside the polygon
                    // Vector u : from a vertex on this side to the meet
                    AffVec u = aff_vec_from_points(poly[i], meet);
                    // Vector v : the two vertices that defined this side
                    AffVec v = aff_vec_from_points(poly[i], poly[i+1]);
                    bool float_error = true;
                    float lambda;                               // scaling factor btwn u and v
                    { // Lambda is just a ratio, but floating point error makes this tricky.
                        // Get this wrong and every once in a while a line is dropped or doubled.
                        /* if(  u.y != 0  ){ lambda = u.y / v.y; } */
                        /* else            { lambda = u.x / v.x; } */
                        float epsilon = 0.01;                       // Floating point error
                        if(  u.x != 0  )     // Use vec.x if non-zero
                        {
                            lambda = u.x / v.x;
                            if(  (v.x > epsilon) || (v.x < -1*epsilon) ) {float_error = false;}
                        }
                        else     // Use vec.y if vec.x is 0
                        {
                            lambda = u.y / v.y;
                            if(  (v.y > epsilon) || (v.y < -1*epsilon) ) {float_error = false;}
                        }
                    }
                    // TODO:
                    // I don't want both lambda=0 and lambda=1, pick one.
                    // The reason is I get two fill lines at the same y-value.
                    // If the color has alpha, then the two lines overlap and it
                    // doesn't look good.
                    // TODO:
                    // The calculation of the meet is *slightly* off. Why?
                    // This causes the occasional fill line to get dropped.
                    if(  float_error == false  )
                    {
                        if(  (lambda>0) && (lambda<=1)  )          // The meet is on the poly seg
                        {
                            meets[meet_cnt] = meet;                 // Store this meet
                            meet_cnt++;                             // Track number of meets
                        }
                    }
                }
                { // Draw the portions of the scan line that are inside the polygon
                    for( int i=0; i<meet_cnt-1; i++ )
                    {
                        // TODO: add an even/odd check on i to catch polygon cutouts
                        SDL_SetRenderDrawColor(ren, 200, 200, 10, 100);     // Set fill color
                        // Very important: do not use meets[i+1].y
                        SDL_RenderDrawLineF(ren, meets[i].x, meets[i].y, meets[i+1].x, meets[i].y);
                    }
                }
                y++;                                            // Advance scanline
            }
        }
        if(1) // DEBUG : stepping line to test my intersection algorithm
        { // scanline : Step line up down with arrow keys instead of looping
            // Find intersection of scanline with each side
            // Make a list of lines out of the polygon sides
            AffLine sides[poly_cnt-1];
            for( int i=0; i<(poly_cnt-1); i++ )
            {
                sides[i] = aff_join_of_points(poly[i], poly[i+1]);
            }
            // Up/Down UI checks that Y is between topmost and botmost
            AffLine scanline = {0, 1, Y};                       // line : y = Y
            int meet_cnt = 0;                                   // count intersections
            AffPoint meets[poly_cnt];                           // at most 1 meet per poly seg
            for( int i=0; i<(poly_cnt-1); i++ )
            {
                AffPoint meet = aff_meet_of_lines(scanline, sides[i]);
                // DEBUG: draw each meet : PASS
                // DEBUG: only draw the meet if it is on the poly seg : PASS
                AffVec u = aff_vec_from_points(poly[i], meet);
                AffVec v = aff_vec_from_points(poly[i], poly[i+1]);
                float lambda;                               // scaling factor btwn u and v
                if(  u.x != 0  ) { lambda = u.x / v.x; }    // Use vec.x if non-zero
                else             { lambda = u.y / v.y; }    // Use vec.y if vec.x is 0
                if(  (lambda >= 0)&&(lambda <= 1)  )        // The meet is on the poly seg
                {
                    // TODO:
                    // Instead of drawing the debug stuff below:
                    // count the number of intersections
                    // for now just handle the case that there are two
                    // if there are not exactly two, do nothing for now
                    // then take those two intersections and draw a line
                    { // highlight meet
                        SDL_SetRenderDrawColor(ren, 100, 200, 10, 180);
                        SDL_FRect r = {meet.x - 2, meet.y - 2, 4, 4};
                        SDL_RenderDrawRectF(ren, &r);
                    }
                    { // highlight polygon point
                        SDL_SetRenderDrawColor(ren, 200, 200, 10, 180);
                        SDL_FRect r = {poly[i].x - 2, poly[i].y - 2, 4, 4};
                        SDL_RenderDrawRectF(ren, &r);
                    }
                    { // draw vector u
                        SDL_RenderDrawLineF(ren, poly[i].x, poly[i].y, meet.x, meet.y);
                    }
                    meets[meet_cnt] = meet;                     // Store meets
                    meet_cnt++;                                 // Track number of meets
                }
            }
            // Draw the scanline
            SDL_SetRenderDrawColor(ren, 200, 200, 200, 180);     // Set fill color
            SDL_RenderDrawLineF(ren, wI.w/2, Y, wI.w, Y);

            /* *************>2 meets?***************
             * if(  meet_cnt >= 2  )                            // 2 or more meets : draw fill
             * Yes, it is possible to have three meets when the scanline hits a
             * vertex. The vertex is the endpoint of two segments, so we have a
             * scenario where lambda is 0 for one of those segments, and lambda
             * is 1 for the other segment. The third meet is on the other line
             * that the scanline passes through.
             *
             * And we do *not* want to draw a line if there are less than 2
             * meets because then we don't have a second point for the line.
             *
             * Note : the for loop ensures there are at least 2 meets.
             * *******************************/
            { // Draw the portions of the scan line that are inside the polygon
                for( int i=0; i<meet_cnt-1; i++ )
                {
                    SDL_SetRenderDrawColor(ren, 200, 100, 10, 180);     // Set fill color
                    // Very important: do not use meets[i+1].y
                    // The line MUST be horizontal. But sometimes the arithmetic
                    // works out to make meets[i+1].y one pixel off from meets[i].y
                    SDL_RenderDrawLineF(ren, meets[i].x, meets[i].y, meets[i+1].x, meets[i].y);
                }
            }

        }
        
        free(poly);                                         // Free mem for polygon
        { // Display to screen
            SDL_RenderPresent(ren);
            SDL_Delay(10);
        }
    }

    // Shutdown
    shutdown();
    return EXIT_SUCCESS;
}

