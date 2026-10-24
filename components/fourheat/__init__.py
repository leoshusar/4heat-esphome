from typing import Generator
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID
from esphome.core import Lambda
from esphome.cpp_generator import LambdaExpression, SafeExpType
from esphome.cpp_types import optional

DEPENDENCIES = ["uart"]

fourheat_ns = cg.esphome_ns.namespace("fourheat")
FourHeat = fourheat_ns.class_("FourHeat", cg.PollingComponent, uart.UARTDevice)

CONF_FOURHEAT_ID = "fourheat_id"

CONF_DATAPOINT = "datapoint"
CONF_PARSER = "parser"
CONF_QUERY_DATAPOINT = "query_datapoint"

CONF_TX_THROTTLE = "tx_throttle"
CONF_RX_TIMEOUT = "rx_timeout"
CONF_RESPONSE_TIMEOUT = "response_timeout"
CONF_RETRY_THROTTLE = "retry_throttle"
CONF_MAX_RETRY_COUNT = "max_retry_count"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(FourHeat),
            cv.Optional(CONF_TX_THROTTLE): cv.int_range(min=5, max=150),
            cv.Optional(CONF_RX_TIMEOUT): cv.int_range(min=50, max=500),
            cv.Optional(CONF_RESPONSE_TIMEOUT): cv.int_range(min=100, max=2500),
            cv.Optional(CONF_RETRY_THROTTLE): cv.int_range(min=500, max=5000),
            cv.Optional(CONF_MAX_RETRY_COUNT): cv.int_range(min=1, max=10)
        }
    )
    .extend(cv.polling_component_schema("15s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_TX_THROTTLE in config:
        cg.add(var.set_tx_throttle(config[CONF_TX_THROTTLE]))
    if CONF_RX_TIMEOUT in config:
        cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))
    if CONF_RESPONSE_TIMEOUT in config:
        cg.add(var.set_response_timeout(config[CONF_RESPONSE_TIMEOUT]))
    if CONF_RETRY_THROTTLE in config:
        cg.add(var.set_retry_throttle(config[CONF_RETRY_THROTTLE]))
    if CONF_MAX_RETRY_COUNT in config:
        cg.add(var.set_max_retry_count(config[CONF_MAX_RETRY_COUNT]))

async def get_parser_expression(value: Lambda, return_type: SafeExpType) -> Generator[LambdaExpression, None, None]:
    return await cg.process_lambda(
        value,
        [
            (
                cg.std_vector.template(cg.uint8).operator("const").operator("ref"),
                "data",
            ),
        ],
        return_type=cg.optional.template(return_type)
    )
