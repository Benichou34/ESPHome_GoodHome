#pragma once
#include "esphome/components/GoodHome/sensorListener.h"
#include "esphome/components/number/number.h"

namespace esphome::goodhome
{
	class GoodHomeNumber : public SensorListener, public number::Number, public Component
	{
	public:
		explicit GoodHomeNumber(const char* tag, float multiplier);

		void publish_raw(const std::string& value) final;
		std::string raw_as_string() const final;
		void dump_config() final;

		void write_value(float value);
		float read_value() const;

	protected:
		void control(float value) final;
	};
} // namespace esphome::goodhome
