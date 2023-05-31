import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import switch
from esphome.const import CONF_ID
from ..const import goodhome_ns

GoodHomeSwitch = goodhome_ns.class_("GoodHomeSwitch", switch.Switch, cg.Component)

def switch_schema(**kwargs) -> cv.Schema:
	return switch.switch_schema(
		GoodHomeSwitch,
		**kwargs
	)

async def new_switch(config, *args):
	var = cg.new_Pvariable(config[CONF_ID], *args)
	await cg.register_component(var, config)
	await switch.register_switch(var, config)
	return var
