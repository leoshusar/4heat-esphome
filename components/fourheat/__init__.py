from typing import Generator
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID
from esphome.core import Lambda
from esphome.cpp_generator import LambdaExpression, SafeExpType

DEPENDENCIES = ["uart"]

fourheat_ns = cg.esphome_ns.namespace("fourheat")
FourHeat = fourheat_ns.class_("FourHeat", cg.PollingComponent, uart.UARTDevice)

CONF_FOURHEAT_ID = "fourheat_id"

CONF_DATAPOINT = "datapoint"
CONF_PARSER = "parser"
CONF_QUERY_DATAPOINT = "query_datapoint"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(FourHeat),
        }
    )
    .extend(cv.polling_component_schema("15s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

async def get_parser_expression(value: Lambda, return_type: SafeExpType) -> Generator[LambdaExpression, None, None]:
    return await cg.process_lambda(
        value,
        [
            (
                cg.std_vector.template(cg.uint8).operator("const").operator("ref"),
                "data",
            ),
        ],
        return_type=cg.std_ns.class_("optional").template(return_type)
    )
