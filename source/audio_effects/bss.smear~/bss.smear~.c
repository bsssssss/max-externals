/**
    @file bss_smear - swoosh
    @ingroup audio_effects
*/

#include "ext.h"      // standard Max include, always required
#include "ext_obex.h" // required for new style Max object
#include "z_dsp.h"

typedef struct _bss_smear {
    t_pxobject obj;
    t_double   prev_samp;
    t_double   g;
} t_bss_smear;

static t_class* s_bss_smear_class;

/*****************************************************************************/

void* bss_smear_new(void);
void  bss_smear_free(t_bss_smear* x);

void bss_smear_dsp64(t_bss_smear* x,
                     t_object*    dsp64,
                     short*       count,
                     double       samplerate,
                     long         maxvectorsize,
                     long         flags);

void bss_smear_perform64(t_bss_smear* x,
                         t_object*    dsp64,
                         double**     ins,
                         long         numins,
                         double**     outs,
                         long         numouts,
                         long         sampleframes,
                         long         flags,
                         void*        userparams);

void bss_smear_assist(t_bss_smear* x,
                      void*        b,
                      long         io,
                      long         index,
                      char*        s);

/*****************************************************************************/

void ext_main(void* r)
{
    t_class* c;
    c = class_new("bss.smear~",
                  (method)bss_smear_new,
                  (method)bss_smear_free,
                  sizeof(t_bss_smear),
                  NULL,
                  0);

    class_dspinit(c);
    class_addmethod(c, (method)bss_smear_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(c, (method)bss_smear_assist, "assist", A_CANT, 0);

    class_register(CLASS_BOX, c);
    s_bss_smear_class = c;
}

void bss_smear_dsp64(
    t_bss_smear* x,
    t_object*    dsp64,
    short*       count,
    double       samplerate,
    long         maxvectorsize,
    long         flags)
{
    dsp_add64(dsp64, (t_object*)x, (t_perfroutine64)bss_smear_perform64, 0, NULL);
    x->prev_samp = 0.0;
    x->g         = 0.0;
}

void bss_smear_perform64(
    t_bss_smear* o,
    t_object*    dsp64,
    double**     ins,
    long         numins,
    double**     outs,
    long         numouts,
    long         sampleframes,
    long         flags,
    void*        userparams)
{
    t_double* in  = ins[0];
    t_double* out = outs[0];
    t_double  g   = *ins[1];

    t_double zi = o->prev_samp;
    t_double xin, x, y;

    while (sampleframes--) {
        xin = *in++;

        x      = xin + zi * g;
        y      = x * -g + zi;
        *out++ = y;

        zi = x;
    }
    o->prev_samp = zi;
}

/* Provides inlets/outlets assistance on hover */
void bss_smear_assist(
    t_bss_smear* x,
    void*        b,
    long         io,
    long         index,
    char*        s)
{
    switch (io) {
        // inlets
        case 1:
            switch (index) {
                case 0:
                    sprintf(s, "(signal) Input");
                    break;
                case 1:
                    sprintf(s, "(signal) g");
                    break;
                case 2:
                    sprintf(s, "(signal) Feedback");
                    break;
            }
            break;
        // outlets
        case 2:
            switch (index) {
                case 0:
                    sprintf(s, "(signal) Output");
                    break;
            }
            break;
    }
}

/* object instanciation routine */
void* bss_smear_new()
{
    t_bss_smear* x = (t_bss_smear*)object_alloc(s_bss_smear_class);

    dsp_setup((t_pxobject*)x, 3);
    outlet_new((t_pxobject*)x, "signal");

    return x;
}

/* object's deletion routine */
void bss_smear_free(t_bss_smear* x)
{
    dsp_free((t_pxobject*)x);
}
