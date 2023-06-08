//
// Created by Malte on 05.05.2023.
//

#ifndef PROTOTYPE_DHT_SENSOR_H
#define PROTOTYPE_DHT_SENSOR_H

#include "DHT.h"


extern DHT dht;

// data structure to hold the sensor data
struct dht_sensor_data {
    float temperature;
    float humidity;
};

// initialize the sensor; must be called before read_dht_22_sensor()
void init_dht_sensor() { dht.begin(); }

// Reads the sensor multiple times and returns the average values_t
dht_sensor_data read_dht_sensor(uint8_t times) {
    dht_sensor_data data{};
    DEBUG("Reading sensor " + String(times) + " times...");
    for (int i = 0; i < times; ++i) { // read multiple times to get a good average value
        float temperature = NAN;
        float humidity = NAN;
        while (isnan(temperature)) temperature = dht.readTemperature(false, true);
        while (isnan(humidity)) humidity = dht.readHumidity(true);
        DEBUG("Read value #" + String(i) + ": " + String(temperature) + "°C, " + String(humidity) + "%");
        data.temperature += temperature;
        data.humidity += humidity;
    }
    data.temperature /= static_cast<float>(times); // return average value
    data.humidity /= static_cast<float>(times); // return average value
    DEBUG("Sensor read.");
    DEBUG("Temperature: " + String(data.temperature) + "°C");
    DEBUG("Humidity: " + String(data.humidity) + "%");
    return data;
}


#endif //PROTOTYPE_DHT_SENSOR_H
