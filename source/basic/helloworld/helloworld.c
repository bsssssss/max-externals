/**
        @file
        helloworld - A max object that says hello

        @ingroup	examples
*/

#include "ext.h" // standard Max include, always required
/*#include "ext_mess.h"*/
#include "ext_obex.h" // required for new style Max object
/*#include "ext_proto.h"*/

// structure de l'objet
typedef struct _helloworld
{
    t_object obj;
    void* m_outlet1;
} t_helloworld;

// Déclaration des fonctions
void* helloworld_new(void);
void helloworld_bang(t_helloworld* x);

// Pointeur vers la classe
static t_class* s_helloworld_class;

// Fonction d'initialisation (quand Max charge l'external)
void ext_main(void* r)
{
    t_class* c;
    c = class_new("helloworld",
                  (method)helloworld_new,
                  (method)NULL,
                  sizeof(t_helloworld),
                  0L,
                  0);
  
    class_addmethod(c, (method)helloworld_bang, "bang", 0);

    class_register(CLASS_BOX, c);
    s_helloworld_class = c;
}

// Routine d'instanciation
void* helloworld_new()
{
    t_helloworld* x = (t_helloworld*)object_alloc(s_helloworld_class);
    x->m_outlet1 = outlet_new((t_object *)x, NULL);
    return x;
}

// Faire qqchose avec les messages 
void helloworld_bang(t_helloworld* x)
{
    object_post((t_object*)x, "Bang me");
    outlet_anything(x->m_outlet1, gensym("Hello Max !"), 0, NULL);
}
