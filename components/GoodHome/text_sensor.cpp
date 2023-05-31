#include "text_sensor.h"
#include "esphome/core/log.h"

namespace
{
	const char* const TAG = "goodhome.text_sensor";

	std::string asciiRemoveAccents(const std::string& str)
	{
		std::string str_a = str;
		for (auto&& c : str_a)
		{
			if (c > 127)
			{
				if (c > 191 && c < 198)
					c = 'A';
				else if (c > 199 && c < 204)
					c = 'E';
				else
					c = '?';
			}
		}

		return str_a;
	}
} // namespace

namespace esphome::goodhome
{
	GoodHomeTextSensor::GoodHomeTextSensor(const char* tag) :
		SensorListener(tag)
	{}

	void GoodHomeTextSensor::publish_raw(const std::string& val)
	{
		ESP_LOGVV(TAG, "Publish text %s: %s", tag().c_str(), val.c_str());
		publish_state(asciiRemoveAccents(val));
	}

	std::string GoodHomeTextSensor::raw_as_string() const
	{
		return get_state();
	}

	void GoodHomeTextSensor::dump_config()
	{
		LOG_TEXT_SENSOR(TAG, "GoodHome Text Sensor", this);
		ESP_LOGCONFIG(TAG, "  Tag name %s", tag().c_str());
	}
} // namespace esphome::goodhome
