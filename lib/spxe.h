#ifndef SIMPLE_PIXEL_ENGINE_H
#define SIMPLE_PIXEL_ENGINE_H

#ifndef PX_TYPE_DEFINED
#define PX_TYPE_DEFINED

typedef struct Px {
    uint8_t r, g, b, a;
} Px;

#endif /* PX_TYPE_DEFINED */

/* spxe core */
int spxeStep(void);
void spxeRender(const Px* pixbuf);
int spxeEnd(Px* pixbuf);
Px* spxeStart(const char* title, const int winwidth, const int winheight, const int scrwidth,   const int scrheight);

void spxeScreenSize(int* widthptr, int* heightptr);
void spxeWindowSize(int* widthptr, int* heightptr);
void spxeBackgroundColor(const Px px);

/* mouse input */
void spxeMousePos(int* xptr, int* yptr);

struct GLFWwindow* spxeGetWindow();

#endif /* SPXE_APPLICATION */