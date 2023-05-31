#include "switch.h"
#include "esphome/core/log.h"

namespace
{
	const char* const TAG = "goodhome.switch";
}

namespace esphome::goodhome
{
	GoodHomeSwitch::GoodHomeSwitch(const char* tag) :
		SensorListener(tag)
	{}

	void GoodHomeSwitch::publish_raw(const std::string& val)
	{
		if (modified())
		{
			ESP_LOGD(TAG, "Ignore switch %s: %s", tag().c_str(), val.c_str());
			return;
		}

		bool newval = (val.size() == 1 && val[0] == '1');
		ESP_LOGVV(TAG, "Publish switch %s: %s", tag().c_str(), ONOFF(state));
		publish_state(newval);
	}

	std::string GoodHomeSwitch::raw_as_string() const
	{
		return this->state ? "1" : "0";
	}

	void GoodHomeSwitch::write_state(bool state)
	{
		ESP_LOGV(TAG, "Setting switch %s: %s", tag().c_str(), ONOFF(state));
		set_modified();
		publish_state(state);
	}

	bool GoodHomeSwitch::get_state() const noexcept
	{
		return this->state;
	}

	void GoodHomeSwitch::dump_config()
	{
		LOG_SWITCH(TAG, "GoodHome Switch", this);
		ESP_LOGCONFIG(TAG, "  Tag name %s", tag().c_str());
	}
} // namespace esphome::goodhome