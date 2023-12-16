#pragma once
#include "esphome/components/GoodHome/sensorListener.h"
#include "esphome/components/select/select.h"
#include <vector>

namespace esphome::goodhome
{
	class GoodHomeSelect : public SensorListener, public select::Select, public Component
	{
	public:
		explicit GoodHomeSelect(const char* tag, const std::vector<size_t>& keys);

		void publish_raw(const std::string& value) final;
		std::string raw_as_string() const final;
		void dump_config() final;

		void write_value(size_t value);
		size_t read_value() const;

	protected:
		void control(const std::string& value) final;

	private:
		std::vector<size_t> m_keys;
	};
} // namespace esphome::goodhome
