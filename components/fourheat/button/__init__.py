from esphome.components import button
import esphome.config_validation as cv
import esphome.codegen as cg

from .. import fourheat_config_validation as fhcv
from .. import (
  fourheat_ns,
  CONF_DATAPOINT,
  CONF_FOURHEAT_ID,
  FourHeat
)

DEPENDENCIES = ["fourheat"]

FourHeatButton = fourheat_ns.class_("FourHeatButton", button.Button, cg.Component)

CONF_PRESS_DATA = "press_data"

CONFIG_SCHEMA = (
    button.button_schema(FourHeatButton)
    .extend(
        {
            cv.GenerateID(CONF_FOURHEAT_ID): cv.use_id(FourHeat),
            cv.Required(CONF_DATAPOINT): fhcv.datapoint,
            
            cv.Optional(CONF_PRESS_DATA): fhcv.data,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await button.new_button(config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_FOURHEAT_ID])
    cg.add(var.set_fourheat_parent(parent))

    cg.add(var.set_datapoint_id(config[CONF_DATAPOINT]))

    if CONF_PRESS_DATA in config:
        cg.add(var.set_press_data(config[CONF_PRESS_DATA]))
