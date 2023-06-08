//
// Created by Malte on 05.05.2023.
//

#ifndef PROTOTYPE_LIGHT_SENSOR_H
#define PROTOTYPE_LIGHT_SENSOR_H

#include <Wire.h>
#include "hp_BH1750.h"


constexpr float LIGHT_SENSOR_CALIBRATION = 90.0f;
extern hp_BH1750 light_sensor;

// Initializes the light sensor; must be called before read_light_sensor()
void init_light_sensor() {
    DEBUG("Initializing light sensor...");
    uint8_t result = light_sensor.begin(BH1750_TO_GROUND);
    assert(result && "Light sensor initialization failed.");
    result = light_sensor.reset();
    assert(result && "Light sensor reset failed.");
    light_sensor.setQuality(BH1750_QUALITY_HIGH2);
    result = light_sensor.calibrateTiming();
    assert(result != BH1750_CAL_COMMUNICATION_ERROR && "Light sensor calibration failed.");
    result = light_sensor.start();
    assert(result && "Light sensor read start failed.");
    DEBUG("Light sensor initialized.");
}

// Reads the light sensor multiple times and returns the average value
float read_light_sensor(uint8_t times) {
    float value = 0;
    DEBUG("Reading sensor " + String(times) + " times...");
    for (int i = 0; i < times; ++i) { // read multiple times to get a good average value
        while (!light_sensor.hasValue(true)); // wait for new value
        auto lux = light_sensor.getLux();
        DEBUG("Read value #" + String(i) + ": " + String(lux) + "lx");
        value += lux;
        light_sensor.adjustSettings(LIGHT_SENSOR_CALIBRATION); // adjust settings to get a good value
        if (i < times - 1) light_sensor.start(); // start sensor again if not last iteration
    }
    value /= static_cast<float>(times); // average value
    DEBUG("Sensor read.");
    DEBUG("Light: " + String(value) + "lx");
    return value;
}


#endif //PROTOTYPE_LIGHT_SENSOR_H
