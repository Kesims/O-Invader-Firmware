#include "watchdog.h"

#include <zephyr/drivers/watchdog.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(watchdog, LOG_LEVEL_DBG);

static const struct device *wdt = DEVICE_DT_GET(DT_NODELABEL(wdt0));

static int wdt_channel_id;

int watchdog_init()
{
    int err;
    if (!device_is_ready(wdt) || wdt == NULL)
    {
        LOG_WRN("Watchdog not ready");
        return -EIO;
    }

    struct wdt_timeout_cfg wdt_config = {
            /* Reset SoC when watchdog timer expires. */
            .flags = WDT_FLAG_RESET_SOC,

            /* Expire watchdog after max window */
            .window.min = 0U,
            .window.max = WDT_MAX_WINDOW};

    wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);
    if (wdt_channel_id < 0)
    {
        LOG_ERR("Watchdog install error.");
        return wdt_channel_id;
    }

    err = wdt_setup(wdt, WDT_OPT_PAUSE_HALTED_BY_DBG);
    if (err)
    {
        LOG_ERR("Watchdog setup error");
        return err;
    }

    return 0;
}

void watchdog_feed()
{
    wdt_feed(wdt, wdt_channel_id);
}

void watchdog_disable()
{
    wdt_disable(wdt);
}