import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor

from esphome.const import (
    CONF_ID,
    CONF_BATTERY_LEVEL,
    CONF_BATTERY_VOLTAGE,
    DEVICE_CLASS_BATTERY,
    STATE_CLASS_MEASUREMENT,
    UNIT_PERCENT,
    UNIT_VOLT,
)

CONF_BATTERY_CHARGE_RATE = "battery_charge_rate"
UNIT_PERCENT_PER_HOUR = "%/h"

CODEOWNERS = ["@ryansch"]
DEPENDENCIES = ["i2c"]

max17048_ns = cg.esphome_ns.namespace("max17048")
MAX17048Component = max17048_ns.class_(
    "MAX17048Component", cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MAX17048Component),
            cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_BATTERY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_LEVEL): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_BATTERY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_CHARGE_RATE): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT_PER_HOUR,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_BATTERY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x36))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if battery_voltage_config := config.get(CONF_BATTERY_VOLTAGE):
        sens = await sensor.new_sensor(battery_voltage_config)
        cg.add(var.set_battery_voltage_sensor(sens))

    if battery_level_config := config.get(CONF_BATTERY_LEVEL):
        sens = await sensor.new_sensor(battery_level_config)
        cg.add(var.set_battery_level_sensor(sens))

    if battery_charge_rate_config := config.get(CONF_BATTERY_CHARGE_RATE):
        sens = await sensor.new_sensor(battery_charge_rate_config)
        cg.add(var.set_battery_charge_rate_sensor(sens))
