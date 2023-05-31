#pragma once
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/goodhome/sensorListener.h"

namespace esphome::goodhome
{
	class GoodHomeBinarySensor : public SensorListener, public binary_sensor::BinarySensor, public Component
	{
	public:
		explicit GoodHomeBinarySensor(const char* tag);

		void publish_raw(const std::string& value) final;
		std::string raw_as_string() const final;
		void dump_config() final;
	};
} // namespace esphome::goodhome
