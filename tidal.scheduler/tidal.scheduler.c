#include "ext.h"
#include "ext_mess.h"
#include "ext_obex.h"
#include "ext_critical.h"

#include <sys/_types/_socklen_t.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>

#include "ext_post.h"
#include "ext_proto.h"
#include "tinyosc.h"

/////////////////////////////////////////////////////////////////////////////////

/* The main data structure
 *
 * Defines a custom data type that groups together related data.
 * This structure contains all the state information needed for one instance of the tidal scheduler.
 */

#define MAX_SCHEDULED_EVENT 64;

typedef struct _scheduled_event {
    t_symbol *address;
    int atom_count;
    t_atom atoms[32];
    double execution_time;
    bool active;
} t_scheduled_event;

typedef struct _tidal_scheduler
{
    // Max object
    t_object obj;
    void* m_outlet;

    // OSC
    int socket_fd;
    long port;

    // Thread
    t_systhread thread;
    int thread_running;

    // Clock
    void* m_clock;

    // The time offset to convert NTP time -> max time
    double time_offset;

    t_atom scheduled_atoms[32];
    int scheduled_atom_count;
    t_symbol* scheduled_address;


} t_tidal_scheduler;

void* tidal_scheduler_new(t_symbol* s, long argc, t_atom* argv);
void tidal_scheduler_free(t_tidal_scheduler* x);

static t_class* tidal_scheduler_class;

/////////////////////////////////////////////////////////////////////////////////

/* This is the entry point of the external.
 *
 *   - Creates a new class called "tidal.scheduler"
 *   - Sets up the constructor and destructor methods
 *   - Registers the class in the system
 */

void ext_main(void* r)
{
    t_class* c;
    c = class_new("tidal.scheduler",
                  (method)tidal_scheduler_new,
                  (method)tidal_scheduler_free,
                  sizeof(t_tidal_scheduler),
                  0L,
                  A_GIMME,
                  0);

    class_register(CLASS_BOX, c);
    tidal_scheduler_class = c;
}

/* Converts an OSC timetag (a 64-bit value) into seconds:
 *
 *    The upper 32 bits represent seconds since January 1, 1900.
 *    The lower 32 bits represent fractions of a second.
 *    It returns a double representing the time in seconds.
 */

double osc_timetag_to_seconds(uint64_t timetag)
{
    // Upper 32 bits (seconds)
    uint32_t seconds = (uint32_t)(timetag >> 32);

    // Lower 32 bits (divide by 2^32)
    uint32_t fraction = (uint32_t)(timetag & 0xFFFFFFFF);

    // Convert to double (secondes + fraction)
    double time_seconds = (double)(seconds) + ((double)fraction / 4294967296.0);

    return time_seconds;
}

/* The function that runs in a separate thread to continuously listen for incoming OSC messages:
 *
 *   - Sets the socket to non-blocking mode
 *   - Enters a loop that continues as long as thread_running is true
 *   - Receives data from the socket
 *
 * If it's an OSC bundle, it:
 *
 *   - Parses the OSC timetag
 *   - Extracts all messages from the bundle
 *   - Converts OSC data types to atoms (the system's internal data format)
 *   - Sends the data through the outlet
 *
 * If no data is available, it sleeps briefly
 */

void* tidal_scheduler_listener(void* arg)
{
    t_tidal_scheduler* x = (t_tidal_scheduler*)arg;
    char buffer[1024];
    struct sockaddr_in sender;
    socklen_t sender_size = sizeof(sender);

    // Make thread non-blocking
    int flags = fcntl(x->socket_fd, F_GETFL, 0);
    fcntl(x->socket_fd, F_SETFL, flags | O_NONBLOCK);

    while (x->thread_running) {
        int bytes_received = recvfrom(x->socket_fd,
                                      buffer,
                                      sizeof(buffer) - 1,
                                      0,
                                      (struct sockaddr*)&sender,
                                      &sender_size);

        if (!(bytes_received > 0)) {
            systhread_sleep(1);
        }
        else {
            // check if bundle
            if (tosc_isBundle(buffer)) {
                tosc_bundle bundle;
                tosc_parseBundle(&bundle, buffer, bytes_received);

                // Get timetag
                uint64_t timetag = tosc_getTimetag(&bundle);
                double osc_seconds = osc_timetag_to_seconds(timetag);
                double max_time_ms = systimer_gettime();
                double max_time_seconds = max_time_ms / 1000.0;

                double target_max_time = osc_seconds + x->time_offset;
                double delay_time = target_max_time - max_time_seconds;
                double delay_time_ms = delay_time * 1000.0;

                /*object_post((t_object*)x, "\n");*/
                /*object_post((t_object*)x, "osc time in seconds: %.3f", osc_seconds);*/
                /*object_post((t_object*)x, "max time in seconds: %.3f", max_time_seconds);*/
                /*object_post((t_object*)x, "target max time    : %.3f", target_max_time);*/
                object_post((t_object*)x, "delay time (ms) -> %.3f", delay_time_ms);

                tosc_message osc;
                while (tosc_getNextMessage(&bundle, &osc)) {
                    const char* address = tosc_getAddress(&osc);
                    const char* format = tosc_getFormat(&osc);

                    t_atom atoms[32];
                    int atom_count = 0;

                    for (int i = 0; format[i] != '\0' && atom_count < 32; i++) {
                        switch (format[i]) {
                            case 'f':
                            {
                                float f = tosc_getNextFloat(&osc);
                                atom_setfloat(&atoms[atom_count++], f);
                                break;
                            }
                            case 'i':
                            {
                                int32_t i = tosc_getNextInt32(&osc);
                                atom_setlong(&atoms[atom_count++], i);
                                break;
                            }
                            case 's':
                            {
                                const char* s = tosc_getNextString(&osc);
                                atom_setsym(&atoms[atom_count++], gensym((char*)s));
                                break;
                            }
                        }
                    }

                    critical_enter(0);

                    if (delay_time_ms <= 0) {
                        outlet_anything(x->m_outlet, gensym((char*)address), atom_count, atoms);
                        object_post((t_object*)x, "delay time was 0ms or less !");
                    }
                    else {
                        x->scheduled_address = gensym((char*)address);
                        x->scheduled_atom_count = atom_count;

                        for (int i = 0; i < atom_count; i++) {
                            x->scheduled_atoms[i] = atoms[i];
                        }

                        clock_fdelay(x->m_clock, delay_time_ms);
                    }
                    critical_exit(0);
                }
            }
        }
    }

    return NULL;
}

/* Callback function for the clock that simply outputs the current system time. */

void tidal_scheduler_tick(t_tidal_scheduler* x)
{
    if (x->scheduled_address && x->scheduled_atom_count > 0) {
        outlet_anything(x->m_outlet, x->scheduled_address, x->scheduled_atom_count, x->scheduled_atoms);
        /*object_post((t_object*)x, "outputing scheduled message !");*/
    }
}

/* The destructor function that cleans up when the object is deleted:
 *
 *    - Stops the listener thread if it's running
 *    - Closes the socket
 *    - Frees the clock object
 */

void tidal_scheduler_free(t_tidal_scheduler* x)
{
    if (x->thread_running) {
        x->thread_running = 0;

        unsigned int ret;
        systhread_join(x->thread, &ret);
    }
    if (x->socket_fd >= 0) {
        close(x->socket_fd);
    }

    object_free(x->m_clock);
}

void set_time_offset(t_tidal_scheduler* t)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    double unix_time = tv.tv_sec + (tv.tv_usec / 1000000.0);
    /*object_post((t_object*)t, "UNIX time at object creation (sec): %.3f", unix_time);*/

    const unsigned long NTP_UNIX_OFFSET = 2208988800UL;
    double ntp_seconds = unix_time + NTP_UNIX_OFFSET;
    double max_seconds = systimer_gettime() / 1000.0;

    t->time_offset = max_seconds - ntp_seconds;
    /*object_post((t_object*)t, "Calculated time offset: %.3f", t->time_offset);*/
}

/* The constructor function that gets called when a new instance of the object is created:
 *
 *     - Allocates memory for the object
 *     - Sets default port to 7400 (can be overridden by arguments)
 *     - Creates an outlet for sending data out
 *     - Creates a clock object for timing purposes
 *     - Sets up a UDP socket on the specified port
 *     - Starts a listener thread that will receive OSC messages
 */

void* tidal_scheduler_new(t_symbol* s, long argc, t_atom* argv)
{
    // Declare a pointer to the t_tidal_scheduler struct
    // Initialize to NULL to avoid problems if allocation fails
    t_tidal_scheduler* x = NULL;

    // Allocate memory for the object and check if allocation is successful
    if ((x = (t_tidal_scheduler*)object_alloc(tidal_scheduler_class))) {

        object_post((t_object*)x, "Initialized");

        set_time_offset(x);

        // Default OSC port
        x->port = 7400;

        // If the object has arguments and the first one is a number
        // replace the default OSC port with the number
        if (argc > 0 && argv[0].a_type == A_LONG) {
            x->port = atom_getlong(argv);
        }

        // Create object outlet
        x->m_outlet = outlet_new(x, NULL);

        // Create clock
        x->m_clock = clock_new((t_object*)x, (method)tidal_scheduler_tick);

        // create UDP socket
        // AF_INET = IPv4, SOCK_DGRAM = UDP mode (without connexion)
        x->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

        // Check if socket creation succeeded
        if (x->socket_fd < 0) {
            object_error((t_object*)x, "Failed to create socket");
            return x;
        }

        // Address config
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(x->port);
        addr.sin_addr.s_addr = INADDR_ANY;

        // Binding the socket to the address and port
        // Make the socket able to receive messages adressed to the port
        if (bind(x->socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            object_error((t_object*)x, "Failed to bind socket to port %ld", x->port);
            close(x->socket_fd);
            x->socket_fd = -1;
            return x;
        }
        object_post((t_object*)x, "Bound to port %ld", x->port);

        // Configure and start
        x->thread_running = 1;

        // Create new thread which will execute tidal_scheduler_listener
        // This thread continuously listen to OSC message coming on the socket
        systhread_create((method)tidal_scheduler_listener, x, 0, 0, 0, &x->thread);
        object_post((t_object*)x, "Listener thread started");
    }

    return x;
}
