#pragma once
#include "esphome/components/GoodHome/sensorListener.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome::goodhome
{
	class GoodHomeSensor : public SensorListener, public sensor::Sensor, public Component
	{
	public:
		explicit GoodHomeSensor(const char* tag, float multiplier);

		void publish_raw(const std::string& value) final;
		std::string raw_as_string() const final;
		void dump_config() final;
	};
} // namespace esphome::goodhome
