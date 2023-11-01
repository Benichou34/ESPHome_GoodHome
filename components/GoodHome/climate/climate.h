#pragma once
#include "esphome/components/GoodHome/number.h"
#include "esphome/components/GoodHome/select.h"
#include "esphome/components/GoodHome/sensor.h"
#include "esphome/components/GoodHome/switch.h"
#include "esphome/components/climate/climate.h"
#include "esphome/core/component.h"

namespace esphome::goodhome
{
	class GoodHomeClimate : public climate::Climate, public Component
	{
	public:
		void setup() final;
		void dump_config() final;

		void setTemperatureSensors(const GoodHomeSensor* current, GoodHomeNumber* target, GoodHomeNumber* override);
		void setActuators(GoodHomeSwitch* manual, GoodHomeSwitch* learning_switch, GoodHomeSelect* target_mode);
		void setActionSensors(const GoodHomeSensor* duty_cyle);

		void update_state();

	protected:
		/// Override control to change settings of the climate device.
		void control(const climate::ClimateCall& call) final;

		/// Return the traits of this controller.
		climate::ClimateTraits traits() final;

	private:
		const GoodHomeSensor* m_current_temperature = nullptr;
		const GoodHomeSensor* m_heat_duty_cycle = nullptr;
		GoodHomeNumber* m_target_temperature = nullptr;
		GoodHomeNumber* m_override_temperature = nullptr;
		GoodHomeSwitch* m_manual_switch = nullptr;
		GoodHomeSwitch* m_learning_switch = nullptr;
		GoodHomeSelect* m_target_mode = nullptr;
	};
} // namespace esphome::goodhome
