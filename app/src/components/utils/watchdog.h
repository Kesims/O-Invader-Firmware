#ifndef BUGPACK_BASE_WATCHDOG_H
#define BUGPACK_BASE_WATCHDOG_H

#define WDT_MAX_WINDOW  10000U

int watchdog_init();
void watchdog_feed();
void watchdog_disable();

#endif //BUGPACK_BASE_WATCHDOG_H
