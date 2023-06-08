//
// Created by Malte on 01.05.2023.
//

#ifndef PROTOTYPE_WIFI_UTILS_H
#define PROTOTYPE_WIFI_UTILS_H

#include <WiFi.h>


namespace wifi_utils {
    void smart_config() {
        WiFiClass::mode(WIFI_AP_STA);
        DEBUG("Starting SmartConfig");
        bool result = WiFi.beginSmartConfig();
        assert(result && "WiFi failed to start SmartConfig");
        uint8_t i = 0;
        status_led_stop();
        status_led_set_frequency(STATUS_LED_DATA.red.bit, 200);
        status_led_start(STATUS_LED_DATA.red.bit);
        while (!WiFi.smartConfigDone()) {
            delay(500);
            DEBUG(".");
            if (i++ > 30) { // 15 seconds
                DEBUG("SmartConfig failed");
                return;
            }
        }
        status_led_stop(STATUS_LED_DATA.red.bit);
        status_led_start(STATUS_LED_DATA.blue.bit);
        DEBUG("SmartConfig done");
    }

    void wait_for_wifi_connection() {
        for (int i = 0; i < 10; i++) {
            if (WiFi.isConnected()) {
                DEBUG("WiFi connected");
                break;
            }
            delay(500);
            DEBUG(".");
        }
    }

    /**
     * @brief Connect to WiFi and start a timer that checks the connection every 5 minutes
     *
     * Tries to connect to the last known WiFi network.
     * If that fails, it starts SmartConfig and tries to connect again.
     */
    void init_wifi() {
#ifdef ENABLEING_THIS_WILL_WHY_EVER_CAUSE_A_CRASH
        WiFi.printDiag(Serial);
#endif
        DEBUG("Connecting to WiFi...");
        WiFi.setSleep(WIFI_PS_MIN_MODEM);
        WiFi.begin();
        wait_for_wifi_connection();
        if (WiFi.isConnected()) {
            DEBUG("WiFi connected: " + WiFi.localIP().toString());
            WiFi.setHostname("AlarmClock");
        } else {
            DEBUG("WiFi connection failed, starting SmartConfig");
            smart_config();
            WiFi.begin();
            wait_for_wifi_connection();
            if (WiFi.isConnected()) {
                DEBUG("WiFi connected: " + WiFi.localIP().toString());
                WiFi.setHostname("AlarmClock");
            } else
                DEBUG("WiFi connection failed");
        }
    }

    /**
     * @brief Check if the device is connected to WiFi
     * @return true if connected, false otherwise
     */
    bool is_wifi_connected() {
        return WiFi.isConnected();
    }

    /**
     * @brief Disable WiFi
     */
    void disable_wifi() {
        WiFi.disconnect(true);
        WiFiClass::mode(WIFI_OFF);
    }
}

using wifi_utils::init_wifi;
using wifi_utils::is_wifi_connected;
using wifi_utils::disable_wifi;


#endif //PROTOTYPE_WIFI_UTILS_H
