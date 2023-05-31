import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import number
from esphome.const import (
	CONF_ID,
	CONF_MAX_VALUE,
	CONF_MIN_VALUE,
	CONF_STEP,
)
from ..const import goodhome_ns

GoodHomeNumber = goodhome_ns.class_("GoodHomeNumber", number.Number, cg.Component)

def number_schema(
	min_value: float = object(),
	max_value: float = object(),
	step: float = object(),
	**kwargs
) -> cv.Schema:
	schema = {}

	if min_value is object():
		schema[cv.Required(CONF_MIN_VALUE)] = cv.float_
	else:
		schema[cv.Optional(CONF_MIN_VALUE, default=min_value)] = cv.float_

	if max_value is object():
		schema[cv.Required(CONF_MAX_VALUE)] = cv.float_
	else:
		schema[cv.Optional(CONF_MAX_VALUE, default=max_value)] = cv.float_

	if step is object():
		schema[cv.Required(CONF_STEP)] = cv.float_
	else:
		schema[cv.Optional(CONF_STEP, default=step)] = cv.float_

	if max_value <= min_value:
		raise cv.Invalid("max_value must be greater than min_value")

	return number.number_schema(
		GoodHomeNumber,
		**kwargs
	).extend(schema)

async def new_number(config, *args):
	var = cg.new_Pvariable(config[CONF_ID], *args)
	await cg.register_component(var, config)
	await number.register_number(
		var,
		config,
		min_value=config[CONF_MIN_VALUE],
		max_value=config[CONF_MAX_VALUE],
		step=config[CONF_STEP],
	)
	return var
