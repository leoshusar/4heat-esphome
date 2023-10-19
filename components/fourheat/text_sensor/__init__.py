from esphome.components import text_sensor
from esphome.const import CONF_OPTIONS
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

FourHeatTextSensor = fourheat_ns.class_("FourHeatTextSensor", text_sensor.TextSensor, cg.Component)

CONFIG_SCHEMA = (
    text_sensor.text_sensor_schema(FourHeatTextSensor)
    .extend(
        {
            cv.GenerateID(CONF_FOURHEAT_ID): cv.use_id(FourHeat),
            cv.Required(CONF_DATAPOINT): fhcv.datapoint,
            cv.Optional(CONF_QUERY_DATAPOINT): fhcv.datapoint,

            cv.Optional(CONF_PARSER): cv.returning_lambda,

            cv.Required(CONF_OPTIONS): fhcv.ensure_dict,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    var = await text_sensor.new_text_sensor(config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_FOURHEAT_ID])
    cg.add(var.set_fourheat_parent(parent))

    cg.add(var.set_datapoint_id(config[CONF_DATAPOINT]))

    if CONF_QUERY_DATAPOINT in config:
        cg.add(var.set_query_datapoint_id(config[CONF_QUERY_DATAPOINT]))

    if CONF_PARSER in config:
        lambda_ = await get_parser_expression(config[CONF_PARSER], int)
        cg.add(var.set_parser(lambda_))

    options = [(int(k), v) for k, v in config[CONF_OPTIONS].items()]
    cg.add(var.set_options(options))
