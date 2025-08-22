#ifndef _DEF_H
#define _DEF_H

#define UNUSED(x)       ((void)(x))

#define MIN(a, b)       ((b) < (a) ? (b) : (a))
#define MAX(a, b)       ((b) > (a) ? (b) : (a))
#define CLAMP(x, a, b)  (MAX(a, MIN(x, b)))

#endif