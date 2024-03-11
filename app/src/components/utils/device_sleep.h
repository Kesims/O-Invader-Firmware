#ifndef BUGPACK_BASE_DEVICE_SLEEP_H
#define BUGPACK_BASE_DEVICE_SLEEP_H

#define DEVICE_SLEEP_TIMER_STEP 10 // in seconds
#define DEVICE_SLEEP_TIMEOUT (60*60) // in seconds, 1 hours

void device_activate();
void device_sleep_init();
void device_sleep();

#endif //BUGPACK_BASE_DEVICE_SLEEP_H
