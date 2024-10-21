#include "teax.h"

#define FREELIST_REF 0

int teax_ref(tea_State* T, int t)
{
    int ref;
    //t = tea_absindex(T, t);
    if(tea_is_nil(T, -1))
    {
        tea_pop(T, 1);     /* remove from stack */
        return TEA_REFNIL; /* `nil' has a unique fixed reference */
    }
    tea_push_integer(T, FREELIST_REF);
    if(!tea_get_field(T, t)) { tea_pop(T, 1), tea_push_nil(T); }    /* get first free element */
    ref = (int)tea_to_integer(T, -1); /* ref = t[FREELIST_REF] */
    tea_pop(T, 1);                   /* remove it from stack */
    if(ref != 0)    /* any free element? */
    {
        tea_push_integer(T, FREELIST_REF);
        tea_push_integer(T, ref);                
        if(!tea_get_field(T, t)) { tea_pop(T, 1), tea_push_nil(T); }          /* remove it from list */
        tea_set_field(T, t); /* (t[FREELIST_REF] = t[ref]) */
    }
    else
    { /* no free elements */
        ref = (int)tea_len(T, t);
        ref++; /* create new reference */
    }
    tea_push_integer(T, ref);
    tea_insert(T, -2);
    tea_set_field(T, t);
    return ref;
}

void teax_unref(tea_State* T, int t, int ref)
{
    if(ref >= 0)
    {
        //t = tea_absindex(T, t);
        tea_push_integer(T, ref);
        tea_push_integer(T, FREELIST_REF);
        if(!tea_get_field(T, t)) { tea_pop(T, 1), tea_push_nil(T); }
        tea_set_field(T, t); /* t[ref] = t[FREELIST_REF] */

        tea_push_integer(T, FREELIST_REF);
        tea_push_integer(T, ref);
        tea_set_field(T, t); /* t[FREELIST_REF] = ref */
    }
}