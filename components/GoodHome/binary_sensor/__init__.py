import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import binary_sensor
from esphome.const import CONF_ID
from ..const import goodhome_ns

AUTO_LOAD = ["binary_sensor"]

GoodHomeBinarySensor = goodhome_ns.class_(
	"GoodHomeBinarySensor", binary_sensor.BinarySensor, cg.Component
)

def binary_sensor_schema(**kwargs) -> cv.Schema:
	return binary_sensor.binary_sensor_schema(
		GoodHomeBinarySensor,
		**kwargs
	)

async def new_binary_sensor(config, *args):
	var = cg.new_Pvariable(config[CONF_ID], *args)
	await cg.register_component(var, config)
	await binary_sensor.register_binary_sensor(var, config)
	return var
