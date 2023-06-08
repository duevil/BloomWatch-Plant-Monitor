#define BLYNK_TEMPLATE_ID "TMPL49CB_VrPp"
#define BLYNK_TEMPLATE_NAME "BloomCube Template"
#define BLYNK_AUTH_TOKEN "bf8vZ3NJLYIIfAFNezcUm4DKp5nY9ehs"
#define BLYNK_NO_BUILTIN   // Disable built-in analog & digital pin operations
#define BLYNK_NO_FLOAT     // Disable float operations

#include <prototype.h>

const gpio_num_t HIBERNATION_WAKEUP_PIN = GPIO_NUM_0;
hp_BH1750 light_sensor{};
DHT dht(GPIO_NUM_17, DHT22);
const uint8_t SOIL_MOISTURE_SENSOR_ADDR = 0x20;
const char *const BLYNK_AUTH = BLYNK_AUTH_TOKEN;
const decltype(LIMITS) LIMITS{
        {2000.0, 250.0, 5000.0, 0.7f},
        {21.5,   15.0,  30.0,    0.1f},
        {60.0,   50.0,  70.0,    0.05f},
        {400.0,  200.0, 500.0,   0.15f},
};
const decltype(STATUS_LED_PINS) STATUS_LED_PINS{
        GPIO_NUM_27,
        GPIO_NUM_25,
        GPIO_NUM_32
};

static constexpr int TIMES = 3;

void setup() {
    init_status_led();
    // indicate that setup is in progress
    status_led_set_frequency(STATUS_LED_DATA.red.bit, 100);
    status_led_start(STATUS_LED_DATA.red.bit);
    setCpuFrequencyMhz(80);
    DEBUG_INIT(SERIAL_BAUD);
    DEBUG("Setup begin");
    get_wakeup_reason();
    // initialize sensors and online services
    init_hibernation_wakeup_pin();
    init_soil_moisture_sensor();
    init_light_sensor();
    init_dht_sensor();
    // indicate that setup is done
    status_led_stop();
    status_led_set_frequency(STATUS_LED_DATA.green.bit, 0);
    status_led_start(STATUS_LED_DATA.green.bit);
    delay(750);
    status_led_stop();
    status_led_set_frequency(STATUS_LED_DATA.green.bit, 1000);
    status_led_set_frequency(STATUS_LED_DATA.blue.bit, 100);
}

void loop() {
    status_led_start(STATUS_LED_DATA.green.bit);
    auto light = read_light_sensor(TIMES);
    auto dht_data = read_dht_sensor(TIMES);
    auto temperature = dht_data.temperature;
    auto humidity = dht_data.humidity;
    auto soil_moisture = read_soil_moisture_sensor(TIMES);
    auto evaluation = evaluate(light, temperature, humidity, soil_moisture);
    status_led_stop();
    status_led_start(STATUS_LED_DATA.blue.bit);
    init_wifi();
    init_blynk();
    if (is_wifi_connected()) {
        DEBUG("Sending data to Blynk...");
        send_blynk(V0, light);
        send_blynk(V1, temperature);
        send_blynk(V2, humidity);
        send_blynk(V3, soil_moisture);
        send_blynk(V4, evaluation);
        DEBUG("Data sent to Blynk.");
    } else {
        DEBUG("WiFi not connected, skipping Blynk update");
    }
    disable_wifi();
    status_led_stop();
    enter_hibernation_when_button_is_pressed();
}