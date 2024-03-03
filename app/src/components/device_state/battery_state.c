#include "battery_state.h"
#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(battery_state, LOG_LEVEL_DBG);

#define ADC_NODE DT_PHANDLE(DT_PATH(zephyr_user), io_channels)
static const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);

#define ADC_RESOLUTION 10
#define ADC_GAIN ADC_GAIN_1_5
#define ADC_REFERENCE ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME_DEFAULT
#define ADC_CHANNEL 1

struct adc_channel_cfg channel1_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .channel_id = ADC_CHANNEL,
    .differential = 0,
#ifdef CONFIG_ADC_NRFX_SAADC
    .input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0 + ADC_CHANNEL,
#endif
};

int16_t adc_buffer[1];

struct adc_sequence sequence = {
    .channels = BIT(ADC_CHANNEL),
    .buffer = adc_buffer,
    .buffer_size = sizeof(adc_buffer),
    .resolution = ADC_RESOLUTION,
    .oversampling = 0,
    .calibrate = false,
};

// Set up a timer that will take care of periodic battery level updates
void battery_update_work_handler(struct k_work *work)
{
    update_battery_level();
}
K_WORK_DEFINE(battery_update_work, battery_update_work_handler);
void battery_update_timer_handler(struct k_timer *timer)
{
    k_work_submit(&battery_update_work);
}
K_TIMER_DEFINE(battery_update_timer, (void (*)(struct k_timer *)) battery_update_work_handler, NULL);

void battery_state_initialize()
{
    if(!device_is_ready(adc_dev)) {
        LOG_ERR("ADC device not ready!\n");
        return;
    }

    int ret = adc_channel_setup(adc_dev, &channel1_cfg);
    if (ret != 0) {
        LOG_ERR("Failed to setup ADC channel 1, error: %d\n", ret);
        return;
    }

    k_timer_start(&battery_update_timer, K_SECONDS(1), K_SECONDS(20));
}

// Mutex to prevent concurrent ADC reads
static struct k_mutex battery_adc_mutex;

// Define a k_poll_signal
static struct k_poll_signal adc_read_done_signal;
static struct k_poll_event  adc_read_done_event = K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY, &adc_read_done_signal);

void update_battery_level()
{
    k_mutex_lock(&battery_adc_mutex, K_FOREVER);

    // Initialize the k_poll_signal
    k_poll_signal_init(&adc_read_done_signal);

    int ret = adc_read_async(adc_dev, &sequence, &adc_read_done_signal);
    if (ret != 0) {
        LOG_ERR("Failed to read ADC channel 1, error: %d\n", ret);
        k_mutex_unlock(&battery_adc_mutex);
        return;
    }

    // Wait for the ADC read operation to complete
    ret = k_poll(&adc_read_done_event, 1, K_FOREVER);
    if (ret != 0 && ret != -11) { // error code -11 is not relevant, and I am not ready to lose my sanity over it
        LOG_ERR("Failed to poll ADC read done signal, error: %d\n", ret);
        k_mutex_unlock(&battery_adc_mutex);
        return;
    }

    // Handle the ADC read result
    int32_t raw_value = adc_buffer[0];
    int32_t mv_value = raw_value;
    adc_raw_to_millivolts(600, ADC_GAIN, ADC_RESOLUTION, &mv_value);
    int32_t real_millivolts = mv_value*2; // There is a voltage divider on the board

//    LOG_INF("Battery voltage: %d mV\n", real_millivolts);

    // Calculate the battery level percentage (max voltage is 4.2V, min voltage is 3.5V)
    uint8_t battery_level = 0;
    if (real_millivolts > 4200) {
        battery_level = 100;
    } else if (real_millivolts < 3500) {
        battery_level = 0;
    } else {
        battery_level = (real_millivolts - 3500) / 7;
    }

    bt_bas_set_battery_level(battery_level);

    k_mutex_unlock(&battery_adc_mutex);
}
