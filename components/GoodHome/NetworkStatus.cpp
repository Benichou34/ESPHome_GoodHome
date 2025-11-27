#include "NetworkStatus.h"
#include "esphome/core/helpers.h"
#include "esphome/core/version.h"
#include <cctype>

namespace
{
	const std::string firmwareVersion("1." ESPHOME_VERSION);
}

namespace esphome::goodhome
{
	NetworkStatus::NetworkStatus() :
		m_uid(esphome::get_mac_address()),
		m_boot(true),
		m_wifiConnected(false),
		m_statusChanged(true)
	{
		std::transform(
			m_uid.begin(),
			m_uid.end(),
			m_uid.begin(),
			[](unsigned char c)
			{
				return std::toupper(c);
			});

		m_moduleId = std::string("module.") + m_uid;
		m_networkId = std::string("network.") + m_uid;
		m_hostId = std::string("host.") + m_uid;
	}

	bool NetworkStatus::isNetworkPresent() const noexcept
	{
		return true;
	}

	bool NetworkStatus::isNetworkOpen() const noexcept
	{
		return false;
	}

	bool NetworkStatus::isWifiConnected() const noexcept
	{
		return m_wifiConnected;
	}

	bool NetworkStatus::isBluetoothOpen() const noexcept
	{
		return false;
	}

	void NetworkStatus::setWifiConnected(bool wifiConnected) noexcept
	{
		if (m_wifiConnected != wifiConnected)
		{
			m_wifiConnected = wifiConnected;
			m_statusChanged = true;
		}
	}

	bool NetworkStatus::isStatusChanged() const noexcept
	{
		return m_statusChanged;
	}

	void NetworkStatus::clearChangedStatus() noexcept
	{
		m_boot = false;
		m_statusChanged = false;
	}

	const std::string& NetworkStatus::moduleID() const noexcept
	{
		return m_moduleId;
	}

	const std::string& NetworkStatus::networkID() const noexcept
	{
		return m_networkId;
	}

	const std::string& NetworkStatus::hostID() const noexcept
	{
		return m_hostId;
	}

	void NetworkStatus::toJson(JsonObject& js) const noexcept
	{
		js["boot"] = m_boot;
		js["firmwareVersion"] = firmwareVersion;

		js["moduleID"] = moduleID();
		js["networkPresent"] = isNetworkPresent();
		js["networkID"] = networkID();
		js["networkOpen"] = isNetworkOpen();

		JsonArray jsModules = js["moduleList"].to<JsonArray>();
		jsModules.add(moduleID());

		js["connectedModules"].to<JsonArray>();
		js["wifiConnected"] = isWifiConnected();
		js["bluetoothOpen"] = isBluetoothOpen();

		js["bluetoothPaired"].to<JsonArray>();
		JsonObject jsTopology = js["topology"].to<JsonObject>();
		jsTopology["sta_uids"].to<JsonArray>();
	}
} // namespace esphome::goodhome
