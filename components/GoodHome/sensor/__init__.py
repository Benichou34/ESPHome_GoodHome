import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID
from ..const import goodhome_ns

GoodHomeSensor = goodhome_ns.class_("GoodHomeSensor", sensor.Sensor, cg.Component)

def sensor_schema(**kwargs) -> cv.Schema:
	return sensor.sensor_schema(
		GoodHomeSensor,
		**kwargs
    )

async def new_sensor(config, *args):
	var = cg.new_Pvariable(config[CONF_ID], *args)
	await cg.register_component(var, config)
	await sensor.register_sensor(var, config)
	return var
