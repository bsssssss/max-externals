/**
        @file bss.automata1D - A 1D cellular automata implementation
        @ingroup lists
*/

#include "ext.h"      // standard Max include, always required
#include "ext_obex.h" // required for new style Max object

// max object struct
typedef struct _bss_automata1D
{
    t_object m_obj; // the object itself (must be first)
    void*    m_outlet1;
    long*    m_ruleset;
} t_bss_automata1D;

void* bss_automata1D_new(void);
void  bss_automata1D_bang(t_bss_automata1D* x);

// global pointer to class definition (setup in ext_main)
static t_class* s_bss_automata1D_class;

// initialization routine (do not change it's name)
void ext_main(void* r)
{
    t_class* c;
    c = class_new("bss.automata1D",
                  (method)bss_automata1D_new,
                  (method)NULL, // The free method
                  sizeof(t_bss_automata1D),
                  0L,
                  0);
    class_addmethod(c, (method)bss_automata1D_bang, "bang", 0);
    class_register(CLASS_BOX, c);
    s_bss_automata1D_class = c;
}

// instanciation routine
void* bss_automata1D_new()
{
    t_bss_automata1D* x = (t_bss_automata1D*)object_alloc(s_bss_automata1D_class);
    x->m_outlet1        = outlet_new((t_object*)x, NULL);
    return x;
}

void bss_automata1D_bang(t_bss_automata1D* x)
{
    outlet_anything(x->m_outlet1,
                    gensym("Hello Max from automata1D !"),
                    0,
                    NULL);
}
