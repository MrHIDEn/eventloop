#include <stdint.h>

/* Layout must match Klin `eventloop.Flag` / emitted `eventloop_Flag`. */
typedef struct {
    int32_t set;
} eventloop_Flag;

static eventloop_Flag g_flag;

eventloop_Flag *flag_cell(void) {
    return &g_flag;
}
