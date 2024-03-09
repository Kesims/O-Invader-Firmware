#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#include "components/bluetooth/bluetooth_core.h"
#include "components/logging/logging.h"
#include "components/device_state/battery_state.h"
#include "components/data_provider/spi_scraper.h"
#include "components/utils/led_indication.h"
#include "data_provider/punch_processing.h"
#include "utils/watchdog.h"
#include "utils/device_config.h"
#include "utils/device_sleep.h"

LOG_MODULE_REGISTER(o_invader_main, LOG_LEVEL_DBG);

int main(void)
{
//    NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
//    NRF_TIMER1->TASKS_CLEAR = 1;
//    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
//    NRF_TIMER1->TASKS_START = 1;
//    NRF_CLOCK->TASKS_HFCLKSTART = 1;
//    int counter = 1000000;
//    while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0 && counter-- > 0);
////    while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
//    NRF_TIMER1->TASKS_CAPTURE[0] = 1;
//
//    printk("HF Clock has started. Startup time: %d uS\n", NRF_TIMER1->CC[0]);
//    printk("All up and running!\n");



    LOG_INF("Starting up...\n");
    flash_storage_init();
    watchdog_init();
    led_indication_init();
    spi_scraper_init();
    logging_initialize();
    bt_core_initialize();
    battery_state_initialize();
    update_battery_level();
    initialize_punch_processing();
    device_sleep_init();

    LOG_INF("All up and running!\n");

    while(1) {
        k_sleep(K_MSEC(5000));
        watchdog_feed(); // TODO make the watchdog properly checkout all threads
    }
}