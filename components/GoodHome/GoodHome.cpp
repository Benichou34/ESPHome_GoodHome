#include "esphome/components/GoodHome/GoodHome.h"
#include "esphome/components/GoodHome/sensorListener.h"
#ifdef USE_HOMEASSISTANT_TIME
#include "esphome/components/homeassistant/time/homeassistant_time.h"
#endif
#include "esphome/components/json/json_util.h"
#include "esphome/components/wifi/wifi_component.h"
#include "esphome/core/log.h"
#ifdef USE_GOODHOME_CLIMATE
#include "esphome/components/GoodHome/climate/climate.h"
#endif

namespace
{
	constexpr char const* TAG = "GoodHome";

	// cmdName
	const std::string cmdName_network_status_changed("system.network_status_changed");           // ESP -> ARM
	const std::string cmdName_check_secure_element_status("system.check_secure_element_status"); // ARM -> ESP
	const std::string cmdName_get_network_status("system.get_network_status");                   // ARM -> ESP
	const std::string cmdName_create_network("system.create_network");                           // ARM -> ESP
	const std::string cmdName_disconnect_from_network("system.disconnect_from_network");         // ARM -> ESP
	const std::string cmdName_scan_for_open_networks("system.scan_for_open_networks");           // ARM -> ESP
	const std::string cmdName_post_URL("system.post_URL");                                       // ARM -> ESP -> WWW
	const std::string cmdName_get_URL("system.get_URL");                                         // ARM -> ESP -> WWW
	const std::string cmdName_disable_TO_bus("system.disable_TO_bus");                           // ARM -> ESP, ESP OFF
	const std::string cmdName_factory_reset("system.system.factory_reset");                      // ARM -> ESP

	// Bluetooth
	const std::string cmdName_send_to_host("system.send_to_host"); // ARM -> ESP -> Bluetooth
	const std::string cmdName_bluetooth_get_paired_devices("system.bluetooth_get_paired_devices");     // ARM -> ESP
	const std::string cmdName_bluetooth_set_open_for_pairing("system.bluetooth_set_open_for_pairing"); // ARM -> ESP
	const std::string cmdName_bluetooth_display_key("system.bluetooth_display_key");                   // ESP -> ARM
	const std::string cmdName_bluetooth_connected("system.bluetooth_connected");                       // ESP -> ARM
	const std::string cmdName_bluetooth_disconnected("system.bluetooth_disconnected");                 // ESP -> ARM
	const std::string cmdName_ble_set_attr("ble_set_attr"); // BLE -> ESP -> ARM
	const std::string cmdName_ble_get_attr("ble_get_attr"); // BLE -> ESP -> ARM
	// system.bluetooth_disconnect
	// system.bluetooth_set_mobile_device_name
	// system.bluetooth_forget

	// WIFI
	const std::string cmdName_WIFI_connect("system.WIFI_connect");       // ARM -> ESP
	const std::string cmdName_WIFI_disconnect("system.WIFI_disconnect"); // ARM -> ESP

	const std::string method_setAttr("setAttr");                         // Set attributes
	const std::string method_getAttr("getAttr");                         // Get attributes
	const std::string method_setSch("setSch");                           // Set scheduler
	const std::string method_getSch("getSch");                           // Get scheduler

	std::string buildCommandFrame(
		const std::string& cmdName,
		const std::string& srcID,
		const std::string& dstID,
		const JsonObject& params = {})
	{
		DynamicJsonDocument doc(10240);

		doc["type"] = "command";
		doc["cmdName"] = cmdName;
		doc["srcID"] = srcID;
		doc["dstID"] = dstID;

		if (params)
			doc["params"] = params;

		std::string output;
		serializeJson(doc, output);
		return output;
	}

	std::string buildReturnFrame(
		const std::string& cmdName,
		unsigned long cmdID,
		const std::string& srcID,
		const std::string& dstID,
		const JsonObject& params = {})
	{
		DynamicJsonDocument doc(10240);

		doc["type"] = "return";
		doc["cmdName"] = cmdName;
		doc["cmdID"] = cmdID;
		doc["srcID"] = srcID;
		doc["dstID"] = dstID;

		if (params)
			doc["params"] = params;

		std::string output;
		serializeJson(doc, output);
		return output;
	}

	std::string buildReturnFrame(
		const std::string& cmdName,
		unsigned long cmdID,
		const std::string& srcID,
		const std::string& dstID,
		const std::string& params)
	{
		DynamicJsonDocument doc(10240);

		doc["type"] = "return";
		doc["cmdName"] = cmdName;
		doc["cmdID"] = cmdID;
		doc["srcID"] = srcID;
		doc["dstID"] = dstID;

		if (!params.empty())
			doc["params"] = params;

		std::string output;
		serializeJson(doc, output);
		return output;
	}
} // namespace

namespace esphome::goodhome
{
	GoodHome::GoodHome() :
#ifdef USE_GOODHOME_CLIMATE
		m_climate(nullptr),
#endif
		m_serial(*this),
		m_forceRefresh(false),
		m_synchroARM(true)
	{}

	void GoodHome::register_listener(SensorListener* listener)
	{
		m_listeners.push_back(listener);
	}

#ifdef USE_GOODHOME_CLIMATE
	void GoodHome::register_climate(GoodHomeClimate* climate)
	{
		m_climate = climate;
	}
#endif

	float GoodHome::get_setup_priority() const
	{
		return setup_priority::HARDWARE;
	}

	SensorListener* GoodHome::getListenerFromTag(const std::string& tag)
	{
		auto it = std::find_if(
			m_listeners.begin(),
			m_listeners.end(),
			[&](const SensorListener* listener)
			{
				return listener->tag() == tag;
			});
		if (it == m_listeners.end())
			return nullptr;

		return *it;
	}

	void GoodHome::interpretIncomingRequest(const JsonObject& jsRequest)
	{
		if (!jsRequest.isNull())
		{
			std::string method = jsRequest["method"].as<std::string>();
			JsonObject jsValue = jsRequest["value"].as<JsonObject>();

			ESP_LOGV(TAG, "Request method: %s", method.c_str());
			if (method == method_setAttr)
			{
				for (JsonPair kv : jsValue)
				{
					ESP_LOGV(TAG, "Value %s: %s", kv.key().c_str(), kv.value().as<const char*>());
					auto it = std::find_if(
						m_listeners.begin(),
						m_listeners.end(),
						[&](const SensorListener* listener)
						{
							return listener->tag().compare(kv.key().c_str()) == 0;
						});
					if (it != m_listeners.end())
						(*it)->publish_raw(kv.value().as<std::string>());
				}

#ifdef USE_GOODHOME_CLIMATE
				if (m_climate)
					m_climate->update_state();
#endif
			}
		}
	}

	void GoodHome::generateOutgoingSetAttrRequest(JsonObject& jsParams)
	{
		JsonObject jsRequest = jsParams.createNestedObject("request");
		jsRequest["method"] = method_setAttr;
		JsonObject jsValue = jsRequest.createNestedObject("value");

		if (m_synchroARM)
		{
#ifdef USE_HOMEASSISTANT_TIME
			time::ESPTime espTime = homeassistant::global_homeassistant_time->now();
			if (espTime.is_valid())
			{
				jsValue["localTime"] = to_string(espTime.timestamp);
				jsValue["daylightSaving"] = espTime.is_dst ? "1" : "0";
			}
#endif

			jsValue["ssid"] = wifi::global_wifi_component->wifi_ssid();
			m_synchroARM = false;
		}

		for (auto& listener : m_listeners)
		{
			if (listener->clear_modified())
				jsValue[listener->tag()] = listener->raw_as_string();
		}
	}

	void GoodHome::generateOutgoingGetAttrRequest(JsonObject& jsParams)
	{
		JsonObject jsRequest = jsParams.createNestedObject("request");
		jsRequest["method"] = method_getAttr;
		JsonArray jsValue = jsRequest.createNestedArray("value");
		for (auto& listener : m_listeners)
			jsValue.add(listener->tag());
	}

	void GoodHome::setup()
	{
		ESP_LOGD(TAG, "Setup start");
#ifdef USE_GOODHOME_CLIMATE
		if (m_climate)
		{
			GoodHomeSensor* currentTemperature = reinterpret_cast<GoodHomeSensor*>(getListenerFromTag("currentTemp"));
			GoodHomeNumber* targetTemperature = reinterpret_cast<GoodHomeNumber*>(getListenerFromTag("targetTemp"));
			GoodHomeNumber* overrideTemperature = reinterpret_cast<GoodHomeNumber*>(getListenerFromTag("overrideTemp"));
			GoodHomeSwitch* manualMode = reinterpret_cast<GoodHomeSwitch*>(getListenerFromTag("noprog"));
			GoodHomeSwitch* learningMode = reinterpret_cast<GoodHomeSwitch*>(getListenerFromTag("selfLearning"));
			GoodHomeSelect* targetMode = reinterpret_cast<GoodHomeSelect*>(getListenerFromTag("targetMode"));

			m_climate->setTemperatureSensors(currentTemperature, targetTemperature, overrideTemperature);
			m_climate->setActuators(manualMode, learningMode, targetMode);
		}
#endif

		ESP_LOGD(TAG, "Setup end");
	}

	void GoodHome::update()
	{
		m_forceRefresh = true;
		m_synchroARM = true;
	}

	void GoodHome::loop()
	{
		m_serial.loop();

		m_networkStatus.setWifiConnected(wifi::global_wifi_component->is_connected());

		if (m_serial.rxJsonAvailable())
		{
			DynamicJsonDocument jsDoc(10240);
			std::string rxJson = m_serial.rxJson();

			deserializeJson(jsDoc, rxJson);

			std::string cmdName = jsDoc["cmdName"];
			unsigned long cmdID = jsDoc["cmdID"];

			ESP_LOGD(TAG, "cmdName: %s", cmdName.c_str());

			std::string frame;
			if (cmdName == cmdName_post_URL)
			{
				std::string params = jsDoc["params"]["postMessage"].as<std::string>();
				deserializeJson(jsDoc, params);
				JsonObject jsRequest = jsDoc["request"].as<JsonObject>();

				interpretIncomingRequest(jsRequest);

				// Build returned params
				JsonObject jsParams = jsDoc.to<JsonObject>();
				JsonObject jsStatus = jsParams.createNestedObject("status");
				jsStatus["result"] = "success";

				bool pendingModifications = false;
				for (const auto& listener : m_listeners)
					pendingModifications |= listener->modified();

				if (!pendingModifications && m_forceRefresh)
				{
					generateOutgoingGetAttrRequest(jsParams);
					m_forceRefresh = false;
				}
				else if (pendingModifications || m_synchroARM)
				{
					generateOutgoingSetAttrRequest(jsParams);
					m_forceRefresh = true;
				}

				params.clear();
				serializeJson(jsParams, params);
				frame = buildReturnFrame(cmdName, cmdID, m_networkStatus.moduleID(), m_networkStatus.hostID(), params);
			}
			else if (cmdName == cmdName_send_to_host)
			{
				JsonObject jsRequest = jsDoc["params"]["request"].as<JsonObject>();
				interpretIncomingRequest(jsRequest);
			}
			else if (cmdName == cmdName_get_network_status)
			{
				JsonObject jsParams = jsDoc.to<JsonObject>();
				m_networkStatus.toJson(jsParams);
				m_networkStatus.clearChangedStatus();

				frame =
					buildReturnFrame(cmdName, cmdID, m_networkStatus.moduleID(), m_networkStatus.hostID(), jsParams);
			}
			else if (cmdName == cmdName_disable_TO_bus)
			{
				ESP_LOGI(TAG, "Enter deep sleep");
				esp_deep_sleep_start();
			}

			if (frame.empty())
				frame = buildReturnFrame(cmdName, cmdID, m_networkStatus.moduleID(), m_networkStatus.hostID());

			if (!frame.empty())
				m_serial.txJson(std::move(frame));
		}
		else if (m_networkStatus.isStatusChanged())
		{
			// Send network status
			DynamicJsonDocument doc(10240);

			JsonObject jsParams = doc.to<JsonObject>();
			m_networkStatus.toJson(jsParams);
			m_networkStatus.clearChangedStatus();

			std::string frame = buildCommandFrame(
				cmdName_network_status_changed,
				m_networkStatus.moduleID(),
				m_networkStatus.hostID(),
				jsParams);
			m_serial.txJson(std::move(frame));
		}
	}

	void GoodHome::dump_config()
	{
		ESP_LOGCONFIG(TAG, "GoodHome:");
		check_uart_settings(115200);
		ESP_LOGCONFIG(TAG, "moduleID: %s", m_networkStatus.moduleID().c_str());
	}
} // namespace esphome::goodhome
