//
// Created by Malte on 28.05.2023.
//

#ifndef PROTOTYPE_EVALUATION_H
#define PROTOTYPE_EVALUATION_H


const extern struct {
    struct {
        float optimal;
        float lower;
        float upper;
        float weight;
    } light, temperature, humidity, soil_moisture;
} LIMITS;

float evaluate(
        float light,
        float temperature,
        float humidity,
        float soil_moisture
) {
    float deviation_light =
            std::max(0.0f, std::min(1.0f,
                                    (std::abs(light - LIMITS.light.optimal)) /
                                    std::abs(LIMITS.light.upper - LIMITS.light.lower)));
    float deviation_temperature =
            std::max(0.0f, std::min(1.0f,
                                    (std::abs(temperature - LIMITS.temperature.optimal)) /
                                    std::abs(LIMITS.temperature.upper - LIMITS.temperature.lower)));
    float deviation_humidity =
            std::max(0.0f, std::min(1.0f,
                                    (std::abs(humidity - LIMITS.humidity.optimal)) /
                                    std::abs(LIMITS.humidity.upper - LIMITS.humidity.lower)));
    float deviation_soil_moisture =
            std::max(0.0f, std::min(1.0f,
                                    (std::abs(soil_moisture - LIMITS.soil_moisture.optimal)) /
                                    std::abs(LIMITS.soil_moisture.upper - LIMITS.soil_moisture.lower)));

    return (5.0f - ((LIMITS.light.weight * deviation_light +
                          LIMITS.temperature.weight * deviation_temperature +
                          LIMITS.humidity.weight * deviation_humidity +
                          LIMITS.soil_moisture.weight * deviation_soil_moisture) /
                         (LIMITS.light.weight +
                          LIMITS.temperature.weight +
                          LIMITS.humidity.weight +
                          LIMITS.soil_moisture.weight)) * 4.0f);
}


#endif //PROTOTYPE_EVALUATION_H
