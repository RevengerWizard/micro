#ifndef _TEAX_H
#define _TEAX_H

#include <tea.h>

/* pre-defined references */
#define TEA_NOREF       (-2)
#define TEA_REFNIL      (-1)

int teax_ref(tea_State* T, int t);
void teax_unref(tea_State* T, int t, int ref);

#endif