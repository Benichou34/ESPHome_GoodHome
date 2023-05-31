#include "number.h"
#include "esphome/core/log.h"

namespace
{
	const char* const TAG = "goodhome.number";
}

namespace esphome::goodhome
{
	GoodHomeNumber::GoodHomeNumber(const char* tag, float multiplier) :
		SensorListener(tag, multiplier)
	{}

	void GoodHomeNumber::publish_raw(const std::string& val)
	{
		if (modified())
		{
			ESP_LOGD(TAG, "Ignore number %s: %s", tag().c_str(), val.c_str());
			return;
		}

		float value = parse_number<float>(val).value_or(0.0f) * multiplier();
		ESP_LOGVV(TAG, "Publish number %s: %f", tag().c_str(), value);
		publish_state(value);
	}

	std::string GoodHomeNumber::raw_as_string() const
	{
		return to_string(static_cast<int>(number::Number::state / multiplier()));
	}

	void GoodHomeNumber::write_value(float value)
	{
		control(value);
	}

	float GoodHomeNumber::read_value() const
	{
		return number::Number::state;
	}

	void GoodHomeNumber::control(float value)
	{
		ESP_LOGV(TAG, "Setting number %s: %f", tag().c_str(), value);
		set_modified();
		publish_state(value);
	}

	void GoodHomeNumber::dump_config()
	{
		LOG_NUMBER(TAG, "GoodHome Number", this);
		ESP_LOGCONFIG(TAG, "  Tag name %s", tag().c_str());
		ESP_LOGCONFIG(TAG, "  Muliplier %f", multiplier());
	}
} // namespace esphome::goodhome
