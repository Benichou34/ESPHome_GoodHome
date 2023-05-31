#include "sensor.h"
#include "esphome/core/log.h"

namespace
{
	const char* const TAG = "goodhome.sensor";
}

namespace esphome::goodhome
{
	GoodHomeSensor::GoodHomeSensor(const char* tag, float multiplier) :
		SensorListener(tag, multiplier)
	{}

	void GoodHomeSensor::publish_raw(const std::string& val)
	{
		float value = parse_number<float>(val).value_or(0.0f) * multiplier();
		ESP_LOGVV(TAG, "Publish sensor %s: %f", tag().c_str(), value);
		if (value != get_state())
			publish_state(value);
	}

	std::string GoodHomeSensor::raw_as_string() const
	{
		return to_string(static_cast<int>(get_state() / multiplier()));
	}

	void GoodHomeSensor::dump_config()
	{
		LOG_SENSOR(TAG, "GoodHome Sensor", this);
		ESP_LOGCONFIG(TAG, "  Tag name %s", tag().c_str());
		ESP_LOGCONFIG(TAG, "  Muliplier %f", multiplier());
	}
} // namespace esphome::goodhome
