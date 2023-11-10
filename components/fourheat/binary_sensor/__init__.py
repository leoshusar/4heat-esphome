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

CONF_OFFLINE_SENSOR = "offline_sensor"

CONFIG_SCHEMA = cv.typed_schema(
    {
        "datapoint": binary_sensor.binary_sensor_schema(FourHeatBinarySensor).extend(
            {
                cv.GenerateID(CONF_FOURHEAT_ID): cv.use_id(FourHeat),
                cv.Required(CONF_DATAPOINT): fhcv.datapoint,
                cv.Optional(CONF_QUERY_DATAPOINT): fhcv.datapoint,

                cv.Optional(CONF_PARSER): cv.returning_lambda,
            }
        )
        .extend(cv.COMPONENT_SCHEMA),

        "offline": binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_PROBLEM,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    },
    lower=True,
    key=CONF_TYPE,
    default_type="datapoint",
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_FOURHEAT_ID])

    # Offline sensor
    if config[CONF_TYPE] == "offline":
        sens = await binary_sensor.new_binary_sensor(config[CONF_OFFLINE_SENSOR])
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
