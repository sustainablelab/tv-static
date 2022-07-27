#ifndef __RAND_H__
#define __RAND_H__
/* *************DOC***************
 * Call rand_init() in setup to seed the random number generator.
 * Call rand_pm(pm) for a random value in the range +pm to -pm.
 * *******************************/
/* *************Example***************
 * Add random values to rectangle x, y, width, and height to make it shake.
 *
 * Make random values that are plus/minus a number of pixels (so a random value
 * centered on zero), and make that number of pixels some fraction of the game
 * window size.
 *
 *          float s = Square_scale;                             // Rename Square_scale as s
 *          float rw = rand_pm(wI.w/100);                       // random in range +/- window w / 200
 *          float rh = rand_pm(wI.h/100);                       // random in range +/- window h / 200
 *          SDL_Rect r = {
 *              .x=(wI.w - rw/2)*(1-1/s)/2,
 *              .y=(wI.h - rh/2)*(1-1/s)/2,
 *              .w=(wI.w + rw)/s,
 *              .h=(wI.h + rh)/s
 *          };
 *
 * Note I add the random value to w and h, but I subtract half the random value
 * from x and y.
 *
 * This is so that the small random amount of growth is offset by
 * the small random amount of translation -- it looks like the shape "center" is
 * not changed. If I do not change x and y, the shape will appear pinned in the
 * topleft corner.
 *
 * If I add to x and y instead of substracting half, the amount of shake does
 * not look like it is scaling as the shape shrinks smaller, it actually looks
 * like it is shaking more and that breaks the illusion the shape is moving
 * further away.
 * *******************************/
#include <stdlib.h>                                             // rand
#include <time.h>

void rand_init(void)
{ // Seed rand()
    srand(time(NULL));                                          // seed
}

float rand_pm(float pm)
{
    /* *************DOC***************
     * Return a random number in range [-pm : +pm].
     * *******************************/
    return (float)rand()*pm*2/RAND_MAX - pm;
}

int rand_0_to_max(int max)
{
    /* *************DOC***************
     * Return a random int in range 0 to max
     * *******************************/
    return rand()*max/RAND_MAX;
    // TODO: test division works the way I expect
}

#endif // __RAND_H__

