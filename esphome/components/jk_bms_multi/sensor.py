import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import UNIT_PERCENT, UNIT_VOLT, ICON_BATTERY

DEPENDENCIES = ["uart"]

jk_ns = cg.esphome_ns.namespace("jk_bms_multi")
JkBmsMulti = jk_ns.class_("JkBmsMulti", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JkBmsMulti),
            cv.Required("soc"): cv.ensure_list(sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                icon=ICON_BATTERY,
                accuracy_decimals=1,
            )),
            cv.Required("voltage"): cv.ensure_list(sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                icon=ICON_BATTERY,
                accuracy_decimals=2,
            )),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[cv.GenerateID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    for i, conf in enumerate(config["soc"]):
        sens = await sensor.new_sensor(conf)
        cg.add(var.soc_sensors[i] = sens)

    for i, conf in enumerate(config["voltage"]):
        sens = await sensor.new_sensor(conf)
        cg.add(var.voltage_sensors[i] = sens)
