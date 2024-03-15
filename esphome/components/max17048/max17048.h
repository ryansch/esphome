#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace max17048 {

// Adapted from https://github.com/adafruit/Adafruit_MAX1704X/blob/afc9f58db9ab1b1a2d565f4ad031c866e2b72a25/Adafruit_MAX1704X.h
static const uint8_t VCELL_REG = 0x02;   // Register that holds cell voltage
static const uint8_t SOC_REG = 0x04;     // Register that holds cell state of charge
static const uint8_t MODE_REG = 0x06;    // Register that manages mode
static const uint8_t VERSION_REG = 0x08; // Register that has IC version
static const uint8_t HIBRT_REG = 0x0A;   // Register that manages hibernation
static const uint8_t CONFIG_REG = 0x0C;  // Register that manages configuration
static const uint8_t VALERT_REG = 0x14;  // Register that holds voltage alert values
static const uint8_t CRATE_REG = 0x16;   // Register that holds cell charge rate
static const uint8_t VRESET_ID_REG = 0x18;  // Register that holds reset voltage setting and semi-unique chip ID
static const uint8_t STATUS_REG = 0x1A;  // Register that holds current alert/status
static const uint8_t CMD_REG = 0xFE;     // Register that can be written for special commands

static const uint8_t ALERTFLAG_SOC_CHANGE = 0x20;      // Alert flag for state-of-charge change
static const uint8_t ALERTFLAG_SOC_LOW = 0x10;         // Alert flag for state-of-charge low
static const uint8_t ALERTFLAG_VOLTAGE_RESET = 0x08;   // Alert flag for voltage reset dip
static const uint8_t ALERTFLAG_VOLTAGE_LOW = 0x04;     // Alert flag for cell voltage low
static const uint8_t ALERTFLAG_VOLTAGE_HIGH = 0x02;    // Alert flag for cell voltage high
static const uint8_t ALERTFLAG_RESET_INDICATOR = 0x01; // Alert flag for IC reset notification


class MAX17048Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  void update() override;

  //bool reset(void);
  //bool clearAlertFlag(uint8_t flags); 

  uint16_t get_ic_version(void);
  uint8_t get_chip_id(void);
  uint16_t get_config(void);

  float get_cell_voltage(void);
  float get_state_of_charge(void);
  float get_charge_rate(void);

  void set_quick_start(bool enable);
  void set_sleep_mode(bool enable);
  bool get_hibernate_status(void);

  void set_battery_voltage_sensor(sensor::Sensor *obj) { this->battery_voltage_sensor_ = obj; }
  void set_battery_level_sensor(sensor::Sensor *obj) { this->battery_level_sensor_ = obj; }
  void set_battery_charge_rate_sensor(sensor::Sensor *obj) { this->battery_charge_rate_sensor_ = obj; }

 protected:
  uint16_t read16_(uint8_t a_register);
  void write16_(uint8_t a_register, uint16_t val);

  sensor::Sensor *battery_voltage_sensor_{nullptr};
  sensor::Sensor *battery_level_sensor_{nullptr};
  sensor::Sensor *battery_charge_rate_sensor_{nullptr};
  uint16_t ic_version_{0};
  uint8_t chip_id_{0};
};

} // namespace max17048
} // namespace esphome
