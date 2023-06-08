//
// Created by Malte on 05.05.2023.
//

#ifndef PROTOTYPE_ESP_HIBERNATION_H
#define PROTOTYPE_ESP_HIBERNATION_H


extern const gpio_num_t HIBERNATION_WAKEUP_PIN;

const char* get_wakeup_reason() {
    auto wakeup_reason = esp_sleep_get_wakeup_cause();
    const char* wakeup_reason_str;
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0:
            wakeup_reason_str = "Hibernation wakeup pin";
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            wakeup_reason_str = "RTC GPIO";
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            wakeup_reason_str = "Timer";
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            wakeup_reason_str = "Touchpad";
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            wakeup_reason_str = "ULP program";
            break;
        default:
            wakeup_reason_str = "Unknown";
            break;
    }
    DEBUG("Wakeup reason: " + String(wakeup_reason_str));
    return wakeup_reason_str;
}

void init_hibernation_wakeup_pin() {
    DEBUG("Initializing hibernation wakeup pin...");
    gpio_reset_pin(HIBERNATION_WAKEUP_PIN);
    gpio_set_direction(HIBERNATION_WAKEUP_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(HIBERNATION_WAKEUP_PIN);
    DEBUG("Hibernation wakeup pin initialized.");
}

void enter_hibernation() {
    DEBUG("Entering hibernation...");
    status_led_set_frequency(STATUS_LED_DATA.green.bit, 50);
    status_led_start(STATUS_LED_DATA.green.bit);

    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF); // turn off the RTC slow memory
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF); // turn off the RTC fast memory
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF); // turn off the XTAL
    esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF); // turn off everything else

    auto err = esp_sleep_enable_ext0_wakeup(HIBERNATION_WAKEUP_PIN, 0); // enable deep sleep wake on HIBERNATION_WAKEUP_PIN low
    assert(err != ESP_ERR_INVALID_ARG && "Pin must be a RTC IO pin.");
    delay(250);
    status_led_stop(STATUS_LED_DATA.green.bit);
    DEBUG("Press the hibernation wakeup button at pin " + String(HIBERNATION_WAKEUP_PIN) + " to wake up.");
    esp_deep_sleep_start(); // start deep sleep
}

void enter_hibernation_when_button_is_pressed() {
    bool button_pressed = gpio_get_level(HIBERNATION_WAKEUP_PIN) == 0;
    if (button_pressed) {
        status_led_set_frequency(STATUS_LED_DATA.red.bit, 0);
        status_led_start(STATUS_LED_DATA.red.bit);
        DEBUG("Waiting for button release...");
        while (gpio_get_level(HIBERNATION_WAKEUP_PIN) == 0) {
            delay(100);
        }
        DEBUG("Button released.");
        status_led_stop(STATUS_LED_DATA.red.bit);
        enter_hibernation();
    }
}


#endif //PROTOTYPE_ESP_HIBERNATION_H
