from esphome.components import select
from esphome.const import CONF_OPTIONS, CONF_OPTIMISTIC
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

FourHeatSelect = fourheat_ns.class_("FourHeatSelect", select.Select, cg.Component)

CONFIG_SCHEMA = (
    select.select_schema(FourHeatSelect)
    .extend(
        {
            cv.GenerateID(CONF_FOURHEAT_ID): cv.use_id(FourHeat),
            cv.Optional(CONF_OPTIMISTIC, default=False): cv.boolean,
            cv.Required(CONF_DATAPOINT): fhcv.datapoint,
            cv.Optional(CONF_QUERY_DATAPOINT): fhcv.datapoint,

            cv.Optional(CONF_PARSER): cv.returning_lambda,

            cv.Required(CONF_OPTIONS): fhcv.ensure_dict,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    options_values = [k for k in config[CONF_OPTIONS].values()]

    var = await select.new_select(config, options=options_values)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_FOURHEAT_ID])
    cg.add(var.set_fourheat_parent(parent))
    cg.add(var.set_optimistic(config[CONF_OPTIMISTIC]))

    cg.add(var.set_datapoint_id(config[CONF_DATAPOINT]))

    options = [(int(k), v) for k, v in config[CONF_OPTIONS].items()]
    cg.add(var.set_options(options))

    if CONF_QUERY_DATAPOINT in config:
        cg.add(var.set_query_datapoint_id(config[CONF_QUERY_DATAPOINT]))
    
    if CONF_PARSER in config:
        lambda_ = await get_parser_expression(config[CONF_PARSER], int)
        cg.add(var.set_parser(lambda_))
