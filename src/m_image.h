#ifndef _M_IMAGE_H
#define _M_IMAGE_H

#include <tea.h>

#include "sera/sera.h"

#define IMAGE_CLASS_NAME   "micro.Image"

typedef struct
{
    sr_Buffer* buf;
} Image;

Image* micro_image_new(tea_State* T);

#endif