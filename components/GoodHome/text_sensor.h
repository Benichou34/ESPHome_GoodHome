#pragma once
#include "esphome/components/GoodHome/sensorListener.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome::goodhome
{
	class GoodHomeTextSensor : public SensorListener, public text_sensor::TextSensor, public Component
	{
	public:
		explicit GoodHomeTextSensor(const char* tag);

		void publish_raw(const std::string& value) final;
		std::string raw_as_string() const final;
		void dump_config() final;
	};
} // namespace esphome::goodhome
