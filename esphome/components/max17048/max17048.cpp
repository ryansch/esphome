#include "max17048.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace max17048 {

static const char *const TAG = "max17048";

void MAX17048Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MAX17048 Component...");

  ic_version_ = this->get_ic_version();
  chip_id_ = this->get_chip_id();

  // if (!this->reset()) {
  //   ESP_LOGE(TAG, "Unable to reset MAX17048");
  //   this->mark_failed();
  //   return;
  // }
}

void MAX17048Component::dump_config() {
  ESP_LOGCONFIG(TAG, "MAX17048:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "  Communication with MAX17048 failed!");
  }
  ESP_LOGCONFIG(TAG, "  IC Version: 0x%04X", ic_version_);
  ESP_LOGCONFIG(TAG, "  Chip ID: 0x%02X", chip_id_);
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Battery Voltage", this->battery_voltage_sensor_);
  LOG_SENSOR("  ", "Battery Level", this->battery_level_sensor_);
}

void MAX17048Component::update() {
  // i2c_transaction([](i2c::I2CDevice *dev) {
  //   MAX17048Component *comp = static_cast<MAX17048Component *>(dev);
  //   comp->update_();
  // });
  if (this->battery_voltage_sensor_ != nullptr) {
    this->battery_voltage_sensor_->publish_state(this->get_cell_voltage());
  }

  if (this->battery_level_sensor_ != nullptr) {
    this->battery_level_sensor_->publish_state(this->get_state_of_charge());
  }

  if (this->battery_charge_rate_sensor_ != nullptr) {
    this->battery_charge_rate_sensor_->publish_state(this->get_charge_rate());
  }
}

uint16_t MAX17048Component::get_ic_version(void) {
  uint16_t version = this->read16_(VERSION_REG);
  return version;
}

uint8_t MAX17048Component::get_chip_id(void) {
  uint8_t id = uint8_t(this->read16_(VRESET_ID_REG) & 0xFF);
  return id;
}

uint16_t MAX17048Component::get_config(void) {
  uint16_t config = this->read16_(CONFIG_REG);
  return config;
}

/*!
 *    @brief  Get battery voltage
 *    @return Floating point value read in Volts
 */
float MAX17048Component::get_cell_voltage(void) {
  uint16_t voltage = this->read16_(VCELL_REG);
  return voltage * 78.125 / 1000000;
}

/*!
 *    @brief  Get battery state in percent (0-100%)
 *    @return Floating point value from 0 to 100.0
 */
float MAX17048Component::get_state_of_charge(void) {
  uint16_t soc = this->read16_(SOC_REG);
  return soc / 256.0;
}

/*!
 *    @brief  Charge or discharge rate of the battery in percent/hour
 *    @return Floating point value from 0 to 100.0% per hour
 */
float MAX17048Component::get_charge_rate(void) {
  uint16_t crate = this->read16_(CRATE_REG);
  return crate * 0.208;
}

/*!
 *    @brief  Read a 16-bit register from the MAX17048
 *    @details The MAX17048 datasheet specifies that all registers must be read as 16-bit words.
 *    @param a_register The register to read from
 *    @return 16-bit value read from the register
 */
uint16_t MAX17048Component::read16_(uint8_t a_register) {
  uint8_t data[2];
  uint16_t output;

  if (this->read_register(a_register, data, sizeof(data), true) != i2c::ERROR_OK) {
    return 0;
  }
  // ESP_LOGVV(TAG, "read16_ 0x%04X data[0] 0x%02X", a_register, data[0]);
  // ESP_LOGVV(TAG, "read16_ 0x%04X data[1] 0x%02X", a_register, data[1]);

  output = (uint16_t(data[0] & 0xFF) << 8) | (data[1] & 0xFF);
  ESP_LOGVV(TAG, "read16_ 0x%04X output 0x%04X", a_register, output);
  return output;
}

/*!
 *    @brief  Write a 16-bit register to the MAX17048
 *    @details The MAX17048 datasheet specifies that all registers must be written as 16-bit words.
 *    @param a_register The register to write to
 *    @param val The 16-bit value to write to the register
 */
void MAX17048Component::write16_(uint8_t a_register, uint16_t val) {}

// bool MAX17048Component::reset(void) {
//   // send reset command, the MAX1704 will reset before ACKing,
//   // so I2C xfer is expected to *fail* with a NACK
//   const uint16_be_t data = 0x5400;
//   if (this->write_register(CMD_REG, &data, 2, true) == i2c::ERROR_OK) {
//     return false;
//   }

//   // loop and attempt to clear alert until success
//   for (uint8_t retries = 0; retries < 3; retries++) {
//     if (clearAlertFlag(MAX1704X_ALERTFLAG_RESET_INDICATOR)) {
//       return true;
//     }
//   }

//   // something didn't work :(
//   return false;

// }

/*!
 *    @brief  Function for clearing an alert flag once it has been handled.
 *    @param flags A byte that can have any number of OR'ed alert flags:
 *    MAX1704X_ALERTFLAG_SOC_CHANGE, MAX1704X_ALERTFLAG_SOC_LOW,
 *    MAX1704X_ALERTFLAG_VOLTAGE_RESET, MAX1704X_ALERTFLAG_VOLTAGE_LOW
 *    MAX1704X_ALERTFLAG_VOLTAGE_HIGH, or MAX1704X_ALERTFLAG_RESET_INDICATOR
 *    @return True if the status register write succeeded
 */
// bool MAX17048Component::clearAlertFlag(uint8_t flags) {
//   // TODO: Create buffer to read flags into
//   this->write_byte_16(this->read_byte_16()
// }

} // namespace max17048
} // namespace esphome
