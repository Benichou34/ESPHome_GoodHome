#include "climate.h"
#include "esphome/core/log.h"

namespace
{
	constexpr char const* TAG = "goodhome.climate";
}

namespace esphome::goodhome
{
	void GoodHomeClimate::setup()
	{}

	void GoodHomeClimate::setTemperatureSensors(
		const GoodHomeSensor* current,
		GoodHomeNumber* target,
		GoodHomeNumber* override)
	{
		m_current_temperature = current;
		m_target_temperature = target;
		m_override_temperature = override;
	}

	void GoodHomeClimate::setActionSensors(const GoodHomeSensor* duty_cyle)
	{
		m_heat_duty_cycle = duty_cyle;
	}

	void
	GoodHomeClimate::setActuators(GoodHomeSwitch* manual, GoodHomeSwitch* learning_switch, GoodHomeSwitch* presence_switch, GoodHomeSelect* target_mode)
	{
		m_manual_switch = manual;
		m_learning_switch = learning_switch;
		m_presence_switch = presence_switch;
		m_target_mode = target_mode;
	}

	void GoodHomeClimate::control(const climate::ClimateCall& call)
	{
		if (call.get_target_temperature().has_value())
		{
			this->target_temperature = call.get_target_temperature().value();
			ESP_LOGV(TAG, "Setting target temperature: %.1f", this->target_temperature);
			if (m_override_temperature)
				m_override_temperature->write_value(this->target_temperature);
		}

		if (call.get_mode().has_value())
		{
			this->mode = call.get_mode().value();
			ESP_LOGV(TAG, "Setting mode: %s", LOG_STR_ARG(climate_mode_to_string(this->mode)));
		}

		if (call.get_preset().has_value())
		{
			this->preset = call.get_preset().value();
			ESP_LOGV(TAG, "Setting preset: %s", LOG_STR_ARG(climate_preset_to_string(this->preset.value())));
		}

		if (m_manual_switch && m_learning_switch && m_presence_switch && m_target_mode && m_target_temperature)
		{
			if (this->mode == climate::CLIMATE_MODE_OFF)
			{
				// Set manual mode
				m_manual_switch->turn_on();
				m_target_mode->write_value(3); // SELECT -> Manual Frost protection
				this->target_temperature = 7.f;
				m_target_temperature->write_value(this->target_temperature);
			}
			else if (this->mode == climate::CLIMATE_MODE_AUTO)
			{
				// Set auto mode
				m_manual_switch->turn_off();
				m_learning_switch->turn_off();
				m_presence_switch->turn_off();

				switch (this->preset.value_or(climate::CLIMATE_PRESET_NONE))
				{
				case climate::CLIMATE_PRESET_SLEEP:
					m_target_mode->write_value(12); // SELECT -> Short absence
					break;

				case climate::CLIMATE_PRESET_AWAY:
					m_target_mode->write_value(5); // SELECT -> Long absence
					break;

				case climate::CLIMATE_PRESET_COMFORT:
					m_target_mode->write_value(9); // SELECT -> Forced Comfort
					break;

				case climate::CLIMATE_PRESET_ECO:
					m_target_mode->write_value(10); // SELECT -> Forced Energy-saving
					break;

				case climate::CLIMATE_PRESET_HOME:
					m_presence_switch->turn_on();
					m_learning_switch->turn_on();
					m_target_mode->write_value(0); // SELECT -> Default
					break;

				case climate::CLIMATE_PRESET_ACTIVITY:
					m_presence_switch->turn_on();
					m_target_mode->write_value(0); // SELECT -> Default
					break;

				default:
					m_target_mode->write_value(0); // Default
					break;
				}
			}
			else if (this->mode == climate::CLIMATE_MODE_HEAT)
			{
				// Set manual mode
				m_manual_switch->turn_on();
				m_learning_switch->turn_off();
				m_presence_switch->turn_off();

				switch (this->preset.value_or(climate::CLIMATE_PRESET_NONE))
				{
				case climate::CLIMATE_PRESET_NONE:
					m_target_mode->write_value(8); // SELECT -> Override
					break;

				case climate::CLIMATE_PRESET_SLEEP:
					m_target_mode->write_value(12); // SELECT -> Short absence
					break;

				case climate::CLIMATE_PRESET_AWAY:
					m_target_mode->write_value(5); // SELECT -> Long absence
					break;

				case climate::CLIMATE_PRESET_COMFORT:
					m_target_mode->write_value(1); // SELECT -> Manual Comfort
					break;

				case climate::CLIMATE_PRESET_ECO:
					m_target_mode->write_value(2); // SELECT ->  Manual Energy-saving
					break;

				case climate::CLIMATE_PRESET_HOME:
					m_presence_switch->turn_on();
					m_learning_switch->turn_on();
					m_target_mode->write_value(0); // SELECT -> Default
					break;

				case climate::CLIMATE_PRESET_ACTIVITY:
					m_presence_switch->turn_on();
					m_target_mode->write_value(0); // SELECT -> Default
					break;

				default:
					m_target_mode->write_value(0); // Default
					break;
				}
			}
		}

		publish_state();
	}

	void GoodHomeClimate::update_state()
	{
		if (m_current_temperature)
			this->current_temperature = m_current_temperature->get_state();

		if (m_target_temperature)
			this->target_temperature = m_target_temperature->read_value();

		if (m_heat_duty_cycle)
		{
			if (m_heat_duty_cycle->get_state() > 0)
				this->action = climate::CLIMATE_ACTION_HEATING;
			else
				this->action = climate::CLIMATE_ACTION_IDLE;
		}
		else
		{
			this->action = climate::CLIMATE_ACTION_OFF;
		}

		if (m_manual_switch && m_learning_switch && m_presence_switch && m_target_mode)
		{
			if (m_learning_switch->get_state())
			{
				this->mode = climate::CLIMATE_MODE_AUTO;
				this->preset = climate::CLIMATE_PRESET_HOME;
			}
			else if (m_presence_switch->get_state())
			{
				this->mode = climate::CLIMATE_MODE_AUTO;
				this->preset = climate::CLIMATE_PRESET_ACTIVITY;
			}
			else
			{
				if (m_manual_switch->get_state())
					this->mode = climate::CLIMATE_MODE_HEAT;
				else
					this->mode = climate::CLIMATE_MODE_AUTO;

				switch (m_target_mode->read_value())
				{
				case 1:  // Manual Comfort
				case 9:  // Forced Comfort
				case 60: // Auto Comfort
					this->preset = climate::CLIMATE_PRESET_COMFORT;
					break;

				case 2:  // Manual Energy-saving
				case 10: // Forced Energy-saving
				case 61: // Auto Energy-saving
					this->preset = climate::CLIMATE_PRESET_ECO;
					break;

				case 5:  // Long absence
					this->preset = climate::CLIMATE_PRESET_AWAY;
					break;

				case 12: // Short absence
					this->preset = climate::CLIMATE_PRESET_SLEEP;
					break;

				case 3: //  Manual Frost protection
					this->mode = climate::CLIMATE_MODE_OFF;
					this->preset = climate::CLIMATE_PRESET_NONE;
					break;

				case 8: // Override
				default:
					this->preset = climate::CLIMATE_PRESET_NONE;
					break;
				}
			}
		}
		else
		{
			this->mode = climate::CLIMATE_MODE_OFF;
			this->preset = climate::CLIMATE_PRESET_NONE;
		}

		publish_state();
	}

	climate::ClimateTraits GoodHomeClimate::traits()
	{
		auto traits = climate::ClimateTraits();
		traits.set_supports_action(true);
		traits.set_supports_current_temperature(true);
		traits.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_HEAT, climate::CLIMATE_MODE_AUTO});
		traits.set_supported_presets(
			{climate::CLIMATE_PRESET_NONE,
			 climate::CLIMATE_PRESET_HOME,
			 climate::CLIMATE_PRESET_AWAY,
			 climate::CLIMATE_PRESET_COMFORT,
			 climate::CLIMATE_PRESET_ECO,
			 climate::CLIMATE_PRESET_SLEEP,
			 climate::CLIMATE_PRESET_ACTIVITY});
		return traits;
	}

	void GoodHomeClimate::dump_config()
	{
		LOG_CLIMATE("", "GoodHome Climate", this);
	}
} // namespace esphome::goodhome
