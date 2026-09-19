#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include <stdint.h>
#include <stdbool.h>

#include "sysctl.h"
#include "constant.h"

typedef struct {
    uint64_t current_us;
    uint64_t previous_us;
    float delta_s;
} time_stamp_t;

void time_stamp_init(time_stamp_t *time_stamp, uint64_t current_time_us);
void time_stamp_update_delta(time_stamp_t *time_stamp, uint64_t current_time_us);
bool time_stamp_check_elapsed(time_stamp_t *time_stamp, uint64_t current_time_us, uint64_t elapsed_time_us);

#endif