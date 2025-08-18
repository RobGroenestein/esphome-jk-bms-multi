import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import CONF_ID, ICON_EMPTY, UNIT_EMPTY

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@RobGroenestein"]

jkbmsmulti_ns = cg.esphome_ns.namespace("jkbmsmulti")
JkBmsMultiComponent = jkbmsmulti_ns.class_("JkBmsMultiComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(JkBmsMultiComponent),
        cv.Optional("uart_id"): cv.use_id(uart.UARTComponent),
        cv.Optional("soc"): cv.ensure_list(sensor.sensor_schema(
            unit_of_measurement="%",
            accuracy_decimals=1,
            icon=ICON_EMPTY,
        )),
        cv.Optional("voltage"): cv.ensure_list(sensor.sensor_schema(
            unit_of_measurement="V",
            accuracy_decimals=2,
            icon=ICON_EMPTY,
        )),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if "uart_id" in config:
        uart_component = await cg.get_variable(config["uart_id"])
        cg.add(var.set_uart_parent(uart_component))

    if "soc" in config:
        soc_sensors = []
        for conf in config["soc"]:
            soc_sensor = await sensor.new_sensor(conf)
            soc_sensors.append(soc_sensor)
        cg.add(var.set_soc_sensors(soc_sensors))

    if "voltage" in config:
        voltage_sensors = []
        for conf in config["voltage"]:
            voltage_sensor = await sensor.new_sensor(conf)
            voltage_sensors.append(voltage_sensor)
        cg.add(var.set_voltage_sensors(voltage_sensors))
