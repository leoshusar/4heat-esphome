from esphome.components import sensor
import esphome.config_validation as cv
import esphome.codegen as cg

from .. import fourheat_config_validation as fhcv
from .. import (
    fourheat_ns,
    CONF_DATAPOINT,
    CONF_FOURHEAT_ID,
    CONF_PARSER,
    CONF_QUERY_DATAPOINT,
    FourHeat,
    get_parser_expression,
)

DEPENDENCIES = ["fourheat"]

FourHeatSensor = fourheat_ns.class_("FourHeatSensor", sensor.Sensor, cg.Component)

CONFIG_SCHEMA = (
    sensor.sensor_schema(FourHeatSensor)
    .extend(
        {
            cv.GenerateID(CONF_FOURHEAT_ID): cv.use_id(FourHeat),
            cv.Required(CONF_DATAPOINT): fhcv.datapoint,
            cv.Optional(CONF_QUERY_DATAPOINT): fhcv.datapoint,

            cv.Optional(CONF_PARSER): cv.returning_lambda,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_FOURHEAT_ID])
    cg.add(var.set_fourheat_parent(parent))

    cg.add(var.set_datapoint_id(config[CONF_DATAPOINT]))

    if CONF_QUERY_DATAPOINT in config:
        cg.add(var.set_query_datapoint_id(config[CONF_QUERY_DATAPOINT]))

    if CONF_PARSER in config:
        lambda_ = await get_parser_expression(config[CONF_PARSER], int)
        cg.add(var.set_parser(lambda_))
