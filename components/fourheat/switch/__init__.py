from esphome.components import switch
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

FourHeatSwitch = fourheat_ns.class_("FourHeatSwitch", switch.Switch, cg.Component)

CONF_ON_DATAPOINT = "on_datapoint"
CONF_OFF_DATAPOINT = "off_datapoint"
CONF_ON_DATA = "on_data"
CONF_OFF_DATA = "off_data"

CONFIG_SCHEMA = (
    switch.switch_schema(FourHeatSwitch)
    .extend(
        {
            cv.GenerateID(CONF_FOURHEAT_ID): cv.use_id(FourHeat),
            cv.Required(CONF_DATAPOINT): fhcv.datapoint,
            cv.Optional(CONF_QUERY_DATAPOINT): fhcv.datapoint,

            cv.Optional(CONF_PARSER): cv.returning_lambda,

            cv.Optional(CONF_ON_DATAPOINT): fhcv.datapoint,
            cv.Optional(CONF_OFF_DATAPOINT): fhcv.datapoint,
            cv.Optional(CONF_ON_DATA): fhcv.data,
            cv.Optional(CONF_OFF_DATA): fhcv.data,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await switch.new_switch(config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_FOURHEAT_ID])
    cg.add(var.set_fourheat_parent(parent))

    cg.add(var.set_datapoint_id(config[CONF_DATAPOINT]))

    if CONF_QUERY_DATAPOINT in config:
        cg.add(var.set_query_datapoint_id(config[CONF_QUERY_DATAPOINT]))

    if CONF_PARSER in config:
        lambda_ = await get_parser_expression(config[CONF_PARSER], bool)
        cg.add(var.set_parser(lambda_))

    if CONF_ON_DATAPOINT in config:
        cg.add(var.set_on_datapoint_id(config[CONF_ON_DATAPOINT]))
    if CONF_OFF_DATAPOINT in config:
        cg.add(var.set_off_datapoint_id(config[CONF_OFF_DATAPOINT]))
    if CONF_ON_DATA in config:
        cg.add(var.set_on_data(config[CONF_ON_DATA]))
    if CONF_OFF_DATA in config:
        cg.add(var.set_off_data(config[CONF_OFF_DATA]))
