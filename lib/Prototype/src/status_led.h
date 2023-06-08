//
// Created by Malte on 05.05.2023.
//

#ifndef PROTOTYPE_STATUS_LED_H
#define PROTOTYPE_STATUS_LED_H


extern const struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} STATUS_LED_PINS;

#ifndef DISABLE_STATUS_LED

namespace status_led {

    constexpr uint8_t STATUS_LED_LEDC_RESOLUTION = 8;
    constexpr uint16_t STATUS_LED_LEDC_FREQUENCY = 5000;

    void status_led_toggle_state(uint8_t color);
    const struct {
        struct {
            const uint8_t bit;
            const uint8_t channel;
            const uint8_t duty;
            TimerHandle_t timer;
            mutable uint32_t frequency;
        } red, green, blue;
    } STATUS_LED_DATA{
            {
                    0b001,
                    0,
                    1 << (STATUS_LED_LEDC_RESOLUTION * 9 / 10),
                    xTimerCreate(
                            "led_red",
                            pdMS_TO_TICKS(250),
                            pdTRUE,
                            nullptr,
                            [](TimerHandle_t timer) {
                                status_led_toggle_state(STATUS_LED_DATA.red.bit);
                            }
                    ),
                    250
            },
            {
                    0b010,
                    1,
                    1 << (STATUS_LED_LEDC_RESOLUTION * 9 / 10),
                    xTimerCreate(
                            "led_green",
                            pdMS_TO_TICKS(250),
                            pdTRUE,
                            nullptr,
                            [](TimerHandle_t timer) {
                                status_led_toggle_state(STATUS_LED_DATA.green.bit);
                            }
                    ),
                    250
            },
            {
                    0b100,
                    2,
                    1 << (STATUS_LED_LEDC_RESOLUTION * 9 / 10),
                    xTimerCreate(
                            "led_blue",
                            pdMS_TO_TICKS(250),
                            pdTRUE,
                            nullptr,
                            [](TimerHandle_t timer) {
                                status_led_toggle_state(STATUS_LED_DATA.blue.bit);
                            }
                    ),
                    250
            }
    };

    void status_led_set_state(uint8_t color, bool state) {
        if (color & STATUS_LED_DATA.red.bit) {
#ifdef USE_LEDC
            ledcWrite(STATUS_LED_DATA.red.channel,
                      state ? STATUS_LED_DATA.red.duty : 1 << STATUS_LED_LEDC_RESOLUTION);
#else
            digitalWrite(STATUS_LED_PINS.red, state ? LOW : HIGH);
#endif
        }
        if (color & STATUS_LED_DATA.green.bit) {
#ifdef USE_LEDC
            ledcWrite(STATUS_LED_DATA.green.channel,
                      state ? STATUS_LED_DATA.green.duty : 1 << STATUS_LED_LEDC_RESOLUTION);
#else
            digitalWrite(STATUS_LED_PINS.green, state ? LOW : HIGH);
#endif
        }
        if (color & STATUS_LED_DATA.blue.bit) {
#ifdef USE_LEDC
            ledcWrite(STATUS_LED_DATA.blue.channel,
                      state ? STATUS_LED_DATA.blue.duty : 1 << STATUS_LED_LEDC_RESOLUTION);
#else
            digitalWrite(STATUS_LED_PINS.blue, state ? LOW : HIGH);
#endif
        }
    }

    uint8_t status_led_get_state(uint8_t color) {
        uint8_t state = 0;
        if (color & STATUS_LED_DATA.red.bit) {
            bool red;
#ifdef USE_LEDC
            red = ledcRead(STATUS_LED_DATA.red.channel) != 1 << STATUS_LED_LEDC_RESOLUTION;
#else
            red = digitalRead(STATUS_LED_PINS.red) == LOW;
#endif
            if (red) {
                state |= STATUS_LED_DATA.red.bit;
            }
        }
        if (color & STATUS_LED_DATA.green.bit) {
            bool green;
#ifdef USE_LEDC
            green = ledcRead(STATUS_LED_DATA.green.channel) != 1 << STATUS_LED_LEDC_RESOLUTION;
#else
            green = digitalRead(STATUS_LED_PINS.green) == LOW;
#endif
            if (green) {
                state |= STATUS_LED_DATA.green.bit;
            }
        }
        if (color & STATUS_LED_DATA.blue.bit) {
            bool blue;
#ifdef USE_LEDC
            blue = ledcRead(STATUS_LED_DATA.blue.channel) != 1 << STATUS_LED_LEDC_RESOLUTION;
#else
            blue = digitalRead(STATUS_LED_PINS.blue) == LOW;
#endif
            if (blue) {
                state |= STATUS_LED_DATA.blue.bit;
            }
        }
        return state;
    }

    void status_led_set_timer_period(uint8_t color, bool ms100) {
        if (color & STATUS_LED_DATA.red.bit && STATUS_LED_DATA.red.frequency > 0) {
            if (ms100) {
                xTimerChangePeriod(STATUS_LED_DATA.red.timer, pdMS_TO_TICKS(100), 0);
            } else {
                xTimerChangePeriod(STATUS_LED_DATA.red.timer, pdMS_TO_TICKS(STATUS_LED_DATA.red.frequency), 0);
            }
        }
        if (color & STATUS_LED_DATA.green.bit && STATUS_LED_DATA.green.frequency > 0) {
            if (ms100) {
                xTimerChangePeriod(STATUS_LED_DATA.green.timer, pdMS_TO_TICKS(100), 0);
            } else {
                xTimerChangePeriod(STATUS_LED_DATA.green.timer, pdMS_TO_TICKS(STATUS_LED_DATA.green.frequency), 0);
            }
        }
        if (color & STATUS_LED_DATA.blue.bit && STATUS_LED_DATA.blue.frequency > 0) {
            if (ms100) {
                xTimerChangePeriod(STATUS_LED_DATA.blue.timer, pdMS_TO_TICKS(100), 0);
            } else {
                xTimerChangePeriod(STATUS_LED_DATA.blue.timer, pdMS_TO_TICKS(STATUS_LED_DATA.blue.frequency), 0);
            }
        }
    }

    void status_led_start(uint8_t color = 0b111) {
        status_led_set_state(color, true);
        status_led_set_timer_period(color, true);
    }

    void status_led_stop(uint8_t color = 0b111) {
        if (color & STATUS_LED_DATA.red.bit) {
            xTimerStop(STATUS_LED_DATA.red.timer, 0);
        }
        if (color & STATUS_LED_DATA.green.bit) {
            xTimerStop(STATUS_LED_DATA.green.timer, 0);
        }
        if (color & STATUS_LED_DATA.blue.bit) {
            xTimerStop(STATUS_LED_DATA.blue.timer, 0);
        }
        status_led_set_state(color, false);
        delay(10);
    }

    void init_status_led() {
#ifdef USE_LEDC
        ledcSetup(STATUS_LED_DATA.red.channel, STATUS_LED_LEDC_FREQUENCY, STATUS_LED_LEDC_RESOLUTION);
        ledcAttachPin(STATUS_LED_PINS.red, STATUS_LED_DATA.red.channel);
        ledcSetup(STATUS_LED_DATA.green.channel, STATUS_LED_LEDC_FREQUENCY, STATUS_LED_LEDC_RESOLUTION);
        ledcAttachPin(STATUS_LED_PINS.green, STATUS_LED_DATA.green.channel);
        ledcSetup(STATUS_LED_DATA.blue.channel, STATUS_LED_LEDC_FREQUENCY, STATUS_LED_LEDC_RESOLUTION);
        ledcAttachPin(STATUS_LED_PINS.blue, STATUS_LED_DATA.blue.channel);
#else
        pinMode(STATUS_LED_PINS.red, OUTPUT);
        pinMode(STATUS_LED_PINS.green, OUTPUT);
        pinMode(STATUS_LED_PINS.blue, OUTPUT);
#endif
        status_led_stop();
    }

    void status_led_set_frequency(uint8_t color, uint32_t frequency) {
        if (color & STATUS_LED_DATA.red.bit) {
            STATUS_LED_DATA.red.frequency = frequency;
        }
        if (color & STATUS_LED_DATA.green.bit) {
            STATUS_LED_DATA.green.frequency = frequency;
        }
        if (color & STATUS_LED_DATA.blue.bit) {
            STATUS_LED_DATA.blue.frequency = frequency;
        }
        status_led_set_timer_period(color, false);
        status_led_stop(color);
    }

    void status_led_toggle_state(uint8_t color) {
        auto state = status_led_get_state(color);
        status_led_set_state(color, state ^ color);
        status_led_set_timer_period(color, !(state & color));
    }

} // namespace status_led

using status_led::STATUS_LED_DATA;
using status_led::init_status_led;
using status_led::status_led_start;
using status_led::status_led_stop;
using status_led::status_led_set_frequency;

#else
#define init_status_led() (void)0
#define status_led_start(x) (void)0
#define status_led_stop(x) (void)0
#define status_led_set_frequency(x, y) (void)0
#endif


#endif //PROTOTYPE_STATUS_LED_H
