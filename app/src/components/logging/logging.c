#include "logging.h"
#include "../../conf.h"

LOG_MODULE_REGISTER(bugpack_logger, LOG_LEVEL_DBG);

void logging_initialize() {
    LOG_INF("----------------------------------------\n");
    LOG_INF("O-Invader Firmware Initialized: %d.%d\n", MAJOR_VERSION, MINOR_VERSION);
    LOG_INF("----------------------------------------\n");
}