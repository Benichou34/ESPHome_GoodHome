#pragma once

#include "esphome/components/uart/uart.h"
#include <array>

namespace esphome::goodhome
{
	class SerialExchange
	{
	public:
		explicit SerialExchange(esphome::uart::UARTDevice& uart);
		~SerialExchange() = default;

		void loop();

		bool rxJsonAvailable() const noexcept;
		std::string rxJson() noexcept;

		void txJson(std::string&&) noexcept;

	private:
		void reply_null_frame();
		void send_txJson();

	private:
		std::array<uint8_t, 10240> m_rx_buffer;
		size_t m_rx_offset;
		size_t m_rx_index;
		uint16_t m_rx_counter;
		uint16_t m_tx_counter;
		std::string m_rx_json;
		std::string m_tx_json;
		esphome::uart::UARTDevice& m_uart;
	};
} // namespace esphome::goodhome