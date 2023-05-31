#pragma once

#include "esphome/components/goodhome/NetworkStatus.h"
#include "esphome/components/goodhome/SerialExchange.h"
#include "esphome/components/goodhome/sensorListener.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"

namespace esphome::goodhome
{
	class SensorListener;
	class GoodHomeClimate;

	class GoodHome : public PollingComponent, public uart::UARTDevice
	{
	public:
		GoodHome();
		~GoodHome() = default;

		void register_listener(SensorListener* listener);
#ifdef USE_GOODHOME_CLIMATE
		void register_climate(GoodHomeClimate* climate);
#endif

		void loop() override;
		void setup() override;
		void update() override;
		void dump_config() override;
		float get_setup_priority() const override;

	private:
		SensorListener* getListenerFromTag(const std::string& tag);
		void interpretIncomingRequest(const JsonObject& jsRequest);
		void generateOutgoingSetAttrRequest(JsonObject& jsParams);
		void generateOutgoingGetAttrRequest(JsonObject& jsParams);

	private:
#ifdef USE_GOODHOME_CLIMATE
		GoodHomeClimate* m_climate;
#endif
		std::vector<SensorListener*> m_listeners;
		SerialExchange m_serial;
		NetworkStatus m_networkStatus;
		bool m_forceRefresh;
		bool m_synchroARM;
	};
} // namespace esphome::goodhome
