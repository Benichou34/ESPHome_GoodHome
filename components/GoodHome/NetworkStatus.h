#pragma once
#include "esphome/components/json/json_util.h"

namespace esphome::goodhome
{
	class NetworkStatus
	{
	public:
		NetworkStatus();
		~NetworkStatus() = default;

		bool isNetworkPresent() const noexcept;
		bool isNetworkOpen() const noexcept;
		bool isWifiConnected() const noexcept;
		bool isBluetoothOpen() const noexcept;

		void setWifiConnected(bool) noexcept;

		bool isStatusChanged() const noexcept;
		void clearChangedStatus() noexcept;

		const std::string& moduleID() const noexcept;
		const std::string& networkID() const noexcept;
		const std::string& hostID() const noexcept;

		void toJson(JsonObject& js) const noexcept;

	private:
		std::string m_uid;
		std::string m_moduleId;
		std::string m_networkId;
		std::string m_hostId;
		bool m_boot;
		bool m_wifiConnected;
		bool m_statusChanged;
	};
} // namespace esphome::goodhome