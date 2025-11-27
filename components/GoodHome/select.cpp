#include "select.h"
#include "esphome/core/log.h"

namespace
{
	const char* const TAG = "goodhome.select";
}

namespace esphome::goodhome
{
	GoodHomeSelect::GoodHomeSelect(const char* tag, const std::vector<size_t>& keys) :
		SensorListener(tag),
		m_keys(keys)
	{}

	void GoodHomeSelect::publish_raw(const std::string& val)
	{
		if (modified())
		{
			ESP_LOGD(TAG, "Ignore select %s: %s", tag().c_str(), val.c_str());
			return;
		}

		size_t key = parse_number<size_t>(val).value_or(0);
		auto it = std::find(m_keys.begin(), m_keys.end(), key);
		if (it == m_keys.end())
		{
			ESP_LOGW(TAG, "Invalid key %s: %d", tag().c_str(), key);
			return;
		}

		std::string value = select::Select::at(std::distance(m_keys.begin(), it)).value();
		ESP_LOGV(TAG, "Publish select %s: %d=%s", tag().c_str(), key, value.c_str());
		publish_state(value);
	}

	std::string GoodHomeSelect::raw_as_string() const
	{
		auto idx = select::Select::active_index();
		if (!idx.has_value())
		{
			ESP_LOGW(TAG, "Invalid index %s", tag().c_str());
			return {};
		}

		return to_string(m_keys.at(idx.value()));
	}

	void GoodHomeSelect::write_value(size_t key)
	{
		auto it = std::find(m_keys.begin(), m_keys.end(), key);
		if (it == m_keys.end())
		{
			ESP_LOGW(TAG, "Invalid key %s: %d", tag().c_str(), key);
			return;
		}

		std::string value = select::Select::at(std::distance(m_keys.begin(), it)).value();
		ESP_LOGV(TAG, "Write select %s: %d=%s", tag().c_str(), key, value.c_str());

		set_modified();
		publish_state(value);
	}

	size_t GoodHomeSelect::read_value() const
	{
		return m_keys.at(select::Select::active_index().value_or(0));
	}

	void GoodHomeSelect::control(const std::string& value)
	{
		auto idx = select::Select::index_of(value);
		if (!idx.has_value())
		{
			ESP_LOGW(TAG, "Invalid option %s: %s", tag().c_str(), value.c_str());
			return;
		}

		ESP_LOGV(TAG, "Setting select %s: %s=%d", tag().c_str(), value.c_str(), m_keys.at(idx.value()));
		set_modified();
		publish_state(value);
	}

	void GoodHomeSelect::dump_config()
	{
		LOG_SELECT(TAG, "GoodHome select", this);
		ESP_LOGCONFIG(TAG, "  Tag name %s", tag().c_str());
		ESP_LOGCONFIG(TAG, "  Options keys:");
		const auto& options = select::Select::traits.get_options();
		for (size_t i = 0; i < m_keys.size(); i++)
		{
			ESP_LOGCONFIG(TAG, "    %d: %s", m_keys.at(i), options.at(i).c_str());
		}
	}
} // namespace esphome::goodhome
