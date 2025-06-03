from esphome.components import climate
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID

from .. import (
    GOODHOME_SCHEMA,
    goodhome_ns,
    CONF_GOODHOME_ID,
)

GoodHomeClimate = goodhome_ns.class_("GoodHomeClimate", climate.Climate, cg.Component)

CONFIG_SCHEMA = climate.climate_schema(GoodHomeClimate).extend(
    GOODHOME_SCHEMA
)

async def to_code(config):
    cg.add_define("USE_GOODHOME_CLIMATE")

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)
    goodhome = await cg.get_variable(config[CONF_GOODHOME_ID])
    cg.add(goodhome.register_climate(var))
