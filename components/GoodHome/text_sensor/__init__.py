import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import text_sensor
from esphome.const import CONF_ID
from ..const import goodhome_ns

GoodHomeTextSensor = goodhome_ns.class_(
	"GoodHomeTextSensor", text_sensor.TextSensor, cg.Component
)

def text_sensor_schema(**kwargs) -> cv.Schema:
	return text_sensor.text_sensor_schema(
		GoodHomeTextSensor,
		**kwargs
	)

async def new_text_sensor(config, *args):
	var = cg.new_Pvariable(config[CONF_ID], *args)
	await cg.register_component(var, config)
	await text_sensor.register_text_sensor(var, config)
	return var
