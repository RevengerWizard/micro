#ifndef _M_IMAGE_H
#define _M_IMAGE_H

#include <tea.h>

#include "sera.h"

#define IMAGE_CLASS_NAME   "micro.Image"

typedef struct
{
    sr_Buffer* buf;
} Image;

sr_Pixel get_color(tea_State* T, int idx);
sr_Rect get_rect(tea_State* T, int idx);
void check_subrect(tea_State* T, int idx, sr_Buffer* b, sr_Rect* r);

Image* micro_image_new(tea_State* T);

#endif