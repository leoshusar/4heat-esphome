from esphome.components import binary_sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_TYPE,
    DEVICE_CLASS_PROBLEM,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

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

FourHeatBinarySensor = fourheat_ns.class_("FourHeatBinarySensor", binary_sensor.BinarySensor, cg.Component)

CONF_TYPE_DATAPOINT = "datapoint"
CONF_TYPE_OFFLINE = "module_offline"

CONFIG_SCHEMA = cv.typed_schema(
    {
        CONF_TYPE_DATAPOINT: binary_sensor.binary_sensor_schema(FourHeatBinarySensor).extend(
            {
                cv.GenerateID(CONF_FOURHEAT_ID): cv.use_id(FourHeat),
                cv.Required(CONF_DATAPOINT): fhcv.datapoint,
                cv.Optional(CONF_QUERY_DATAPOINT): fhcv.datapoint,

                cv.Optional(CONF_PARSER): cv.returning_lambda,
            }
        )
        .extend(cv.COMPONENT_SCHEMA),

        CONF_TYPE_OFFLINE: binary_sensor.binary_sensor_schema(
            FourHeatBinarySensor,
            device_class=DEVICE_CLASS_PROBLEM,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        )
        .extend(
            {
                cv.GenerateID(CONF_FOURHEAT_ID): cv.use_id(FourHeat),
            }
        )
        .extend(cv.COMPONENT_SCHEMA),
    },
    lower=True,
    key=CONF_TYPE,
    default_type=CONF_TYPE_DATAPOINT,
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_FOURHEAT_ID])

    # Offline sensor
    if config[CONF_TYPE] == CONF_TYPE_OFFLINE:
        sens = await binary_sensor.new_binary_sensor(config)
        cg.add(parent.set_module_offline_sensor(sens))
    # Datapoint sensor
    else:
        var = await binary_sensor.new_binary_sensor(config)
        await cg.register_component(var, config)

        cg.add(var.set_fourheat_parent(parent))

        cg.add(var.set_datapoint_id(config[CONF_DATAPOINT]))

        if CONF_QUERY_DATAPOINT in config:
            cg.add(var.set_query_datapoint_id(config[CONF_QUERY_DATAPOINT]))

        if CONF_PARSER in config:
            lambda_ = await get_parser_expression(config[CONF_PARSER], bool)
            cg.add(var.set_parser(lambda_))
