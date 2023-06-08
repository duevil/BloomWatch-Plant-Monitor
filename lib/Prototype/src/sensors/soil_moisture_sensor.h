//
// Created by Malte on 25.05.2023.
//

#ifndef PROTOTYPE_SOIL_MOISTURE_SENSOR_H
#define PROTOTYPE_SOIL_MOISTURE_SENSOR_H


extern const uint8_t SOIL_MOISTURE_SENSOR_ADDR;

namespace soil_moisture_sensor {
    uint16_t readI2CRegister16bit(uint8_t addr, uint8_t reg) {
        Wire.beginTransmission(addr);
        Wire.write(reg);
        Wire.endTransmission();
        delay(1100);
        Wire.requestFrom(addr, (uint8_t) 2);
        uint16_t value = Wire.read() << 8;
        value |= Wire.read();
        return value;
    }

    void writeI2CRegister8bit(uint8_t addr, uint8_t value) {
        Wire.beginTransmission(addr);
        Wire.write(value);
        Wire.endTransmission();
    }

    void init_soil_moisture_sensor() {
        Wire.begin();
        DEBUG("Resetting soil moisture sensor...");
        writeI2CRegister8bit(SOIL_MOISTURE_SENSOR_ADDR, 6);
        DEBUG("Reset done. Soil moisture sensor initialized.");
    }

    float read_soil_moisture_sensor(uint8_t times) {
        float value = 0;
        DEBUG("Reading sensor " + String(times) + " times...");
        for (int i = 0; i < times; ++i) { // read multiple times to get a good average value
            uint16_t capacity = UINT16_MAX;
            while (capacity == UINT16_MAX) {
                capacity = readI2CRegister16bit(SOIL_MOISTURE_SENSOR_ADDR, 0);
                delay(10); //allow time for measurement
            }
            DEBUG("Read value #" + String(i) + ": " + String(capacity) + "pF");
            value += static_cast<float>(capacity);
        }
        value /= static_cast<float>(times); // average value
        DEBUG("Sensor read.");
        DEBUG("Soil moisture: " + String(value) + "pF");
        return value;
    }
}

using soil_moisture_sensor::init_soil_moisture_sensor;
using soil_moisture_sensor::read_soil_moisture_sensor;


#endif //PROTOTYPE_SOIL_MOISTURE_SENSOR_H
