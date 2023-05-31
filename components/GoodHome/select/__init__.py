import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import select
from esphome.const import CONF_OPTIONS, CONF_ID
from ..const import goodhome_ns

GoodHomeSelect = goodhome_ns.class_("GoodHomeSelect", select.Select, cg.Component)

def ensure_option_map(value):
	cv.check_not_templatable(value)
	option = cv.All(cv.int_range(0, 2**8 - 1))
	mapping = cv.All(cv.string_strict)
	options_map_schema = cv.Schema({option: mapping})
	value = options_map_schema(value)

	all_values = list(value.keys())
	unique_values = set(value.keys())
	if len(all_values) != len(unique_values):
		raise cv.Invalid("Mapping values must be unique.")

	return value

def select_schema(
	options: dict = object(),
	**kwargs
) -> cv.Schema:
	schema = {}

	if options is object():
		schema[cv.Required(CONF_OPTIONS)] = ensure_option_map
	else:
		schema[cv.Optional(CONF_OPTIONS, default=options)] = ensure_option_map

	return select.select_schema(
		GoodHomeSelect,
		**kwargs
	).extend(schema)

async def new_select(config, *args):
	options_map = config[CONF_OPTIONS]
	var = cg.new_Pvariable(config[CONF_ID], *args, list(options_map.keys()))
	await cg.register_component(var, config)
	await select.register_select(var, config, options=list(options_map.values()))
	return var
