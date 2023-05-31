#include "sensorListener.h"
#include <cmath>

namespace esphome::goodhome
{
	SensorListener::SensorListener(const std::string& tag, float multiplier) :
		m_tag(tag),
		m_multiplier(multiplier),
		m_modified(false)
	{
		if (m_multiplier == 0 || std::isnan(m_multiplier))
			m_multiplier = 1;
	}

	const std::string& SensorListener::tag() const noexcept
	{
		return m_tag;
	}

	float SensorListener::multiplier() const noexcept
	{
		return m_multiplier;
	}

	bool SensorListener::modified() const noexcept
	{
		return m_modified;
	}

	bool SensorListener::clear_modified() noexcept
	{
		bool flag = m_modified;
		m_modified = false;
		return flag;
	}

	void SensorListener::set_modified() noexcept
	{
		m_modified = true;
	}
} // namespace esphome::goodhome
