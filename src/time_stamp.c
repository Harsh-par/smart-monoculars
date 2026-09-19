#include "time_stamp.h"

void time_stamp_init(time_stamp_t *time_stamp, uint64_t current_time_us)
{
    time_stamp->current_us  = current_time_us;
    time_stamp->previous_us = current_time_us;
    time_stamp->delta_s     = (time_stamp->current_us - time_stamp->previous_us) * MICROSECOND_TO_SECOND;
}

void time_stamp_update_delta(time_stamp_t *time_stamp, uint64_t current_time_us)
{
    time_stamp->current_us  = current_time_us;
    time_stamp->delta_s     = (time_stamp->current_us - time_stamp->previous_us) * MICROSECOND_TO_SECOND;
    time_stamp->previous_us = time_stamp->current_us; 
}

bool time_stamp_check_elapsed(time_stamp_t *time_stamp, uint64_t current_time_us, uint64_t elapsed_time_us)
{
    if(current_time_us - time_stamp->previous_us >= elapsed_time_us) 
    {
        time_stamp->previous_us = current_time_us; 
        return true;
    }

    return false;
}