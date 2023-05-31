#include "binary_sensor.h"
#include "esphome/core/log.h"

namespace
{
	const char* const TAG = "goodhome.binary_sensor";
}

namespace esphome::goodhome
{
	GoodHomeBinarySensor::GoodHomeBinarySensor(const char* tag) :
		SensorListener(tag)
	{}

	void GoodHomeBinarySensor::publish_raw(const std::string& val)
	{
		bool value = (val.size() == 1 && val[0] == '1');
		ESP_LOGVV(TAG, "Publish binary %s: %s", tag().c_str(), TRUEFALSE(value));
		publish_state(value);
	}

	std::string GoodHomeBinarySensor::raw_as_string() const
	{
		return binary_sensor::BinarySensor::state ? "1" : "0";
	}

	void GoodHomeBinarySensor::dump_config()
	{
		LOG_BINARY_SENSOR(TAG, "GoodHome Binary Sensor", this);
		ESP_LOGCONFIG(TAG, "  Tag name %s", tag().c_str());
	}
} // namespace esphome::goodhome
