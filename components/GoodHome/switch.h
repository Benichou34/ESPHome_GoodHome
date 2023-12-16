#pragma once
#include "esphome/components/GoodHome/sensorListener.h"
#include "esphome/components/switch/switch.h"

namespace esphome::goodhome
{
	class GoodHomeSwitch : public SensorListener, public switch_::Switch, public Component
	{
	public:
		explicit GoodHomeSwitch(const char* tag);

		void publish_raw(const std::string& value) final;
		std::string raw_as_string() const final;
		void dump_config() final;

		bool get_state() const noexcept;

	protected:
		void write_state(bool state) final;
	};

} // namespace esphome::goodhome