//
// Created by Malte on 05.05.2023.
//

#ifndef PROTOTYPE_BLYNK_H
#define PROTOTYPE_BLYNK_H

#include <BlynkSimpleEsp32.h>


extern const char *const BLYNK_AUTH;

void init_blynk() {
    DEBUG("Connecting to Blynk...");
    Blynk.config(BLYNK_AUTH);
    Blynk.connect();
    uint8_t i = 0;
    while (!Blynk.connected()) {
        delay(500);
        DEBUG(".");
        if (i++ > 20) { // 10 seconds
            DEBUG("Blynk connection failed");
            return;
        }
    }
    DEBUG("Blynk connected");
}

template<typename T>
void send_blynk(int pin, T value) {
    auto v = String(value);
    DEBUG("Sending to Blynk at pin " + String(pin) + ": " + v);
    Blynk.virtualWrite(pin, v);
}


#endif //PROTOTYPE_BLYNK_H
