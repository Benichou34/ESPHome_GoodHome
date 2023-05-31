#pragma once
#include <string>

namespace esphome::goodhome
{
	class SensorListener
	{
	public:
		SensorListener(const std::string& tag, float multiplier = 1);

		virtual void publish_raw(const std::string& value) = 0;
		virtual std::string raw_as_string() const = 0;

		const std::string& tag() const noexcept;
		float multiplier() const noexcept;

		bool modified() const noexcept;
		bool clear_modified() noexcept;

	protected:
		void set_modified() noexcept;

	private:
		std::string m_tag;
		float m_multiplier;
		bool m_modified;
	};
} // namespace esphome::goodhome
