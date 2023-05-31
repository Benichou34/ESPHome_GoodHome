#include "SerialExchange.h"
#include "esphome/core/log.h"
#include <inttypes.h>
#include <string_view>

namespace
{
	constexpr char const* TAG = "GoodHome";

	constexpr std::string_view header("___");
	constexpr uint8_t flag_frame = 0x00;
	constexpr uint8_t flag_check_counter = 0x01;

	uint8_t checksum1(const std::string_view& buffer)
	{
		size_t checksum = 0;
		for (size_t i = 0; i < buffer.size(); ++i)
			checksum += static_cast<uint8_t>(buffer[i]) * (i + 1);
		return static_cast<uint8_t>(checksum & 0xFF);
	}

	uint8_t checksum2(const std::string_view& buffer)
	{
		size_t checksum = 0;
		for (auto c : buffer)
			checksum += static_cast<uint8_t>(c);
		return static_cast<uint8_t>(checksum & 0xFF);
	}
} // namespace

namespace esphome::goodhome
{
	SerialExchange::SerialExchange(esphome::uart::UARTDevice& uart) :
		m_rx_offset(0),
		m_rx_index(0),
		m_rx_counter(0),
		m_tx_counter(0),
		m_uart(uart)
	{}

	void SerialExchange::loop()
	{
		while (m_uart.available() > 0)
		{
			m_rx_buffer[m_rx_index] = static_cast<uint8_t>(m_uart.read());
			++m_rx_index;

			if (m_rx_index == m_rx_buffer.size())
			{
				ESP_LOGE(TAG, "RX buffer overrun (%d)", m_rx_buffer.size());
				m_rx_offset = 0;
				m_rx_index = 0;
			}
		}

		if ((m_rx_index - m_rx_offset) > 5)
		{
			std::string_view buffer_view(reinterpret_cast<const char*>(m_rx_buffer.data()), m_rx_index);

			size_t pos_begin = buffer_view.find(header, m_rx_offset);
			if (pos_begin != std::string_view::npos)
			{
				m_rx_offset = pos_begin;
				pos_begin += header.size();

				// Looking for end of frame
				size_t pos_end = buffer_view.find(header, pos_begin);
				if (pos_end == std::string_view::npos)
					pos_end = m_rx_index;

				std::string_view frame_view(
					reinterpret_cast<const char*>(m_rx_buffer.data() + pos_begin),
					pos_end - pos_begin);
				if (frame_view.size() > 2)
				{
					m_rx_counter = (frame_view[1] << 8) | frame_view[2];
					if (frame_view[0] == flag_check_counter)
					{
						if (m_rx_counter != m_tx_counter)
						{
							ESP_LOGE(TAG, "Counter mismatch: %hu != %hu", m_rx_counter, m_tx_counter);
						}
						m_rx_offset += 6;
					}
					else if (frame_view.size() > 8)
					{
						size_t length = (static_cast<uint8_t>(frame_view[3]) << 16) |
						                (static_cast<uint8_t>(frame_view[4]) << 8) |
						                static_cast<uint8_t>(frame_view[5]);
						if (frame_view.size() >= (length + 8))
						{
							m_rx_offset += 9 + length + 2;

							auto tail = frame_view.substr(frame_view.size() - 2);
							frame_view.remove_prefix(6);
							frame_view.remove_suffix(2);

							uint8_t checksum_1 = checksum1(frame_view);
							uint8_t checksum_2 = checksum2(frame_view);

							if (checksum_1 == (tail[0] & 0xFF) && checksum_2 == (tail[1] & 0xFF))
							{
								if (m_rx_counter == 0)
									reply_null_frame();
								else
									m_rx_json = frame_view;
							}
							else
							{
								ESP_LOGE(
									TAG,
									"Bad checksum %d %02X!=%02X, %02X!=%02X",
									frame_view.size(),
									static_cast<int>(checksum_1),
									static_cast<int>(tail[0]),
									static_cast<int>(checksum_2),
									static_cast<int>(tail[1]));
							}
						}
					}

					if (m_rx_offset == m_rx_index)
					{
						// Reset Buffer
						m_rx_offset = 0;
						m_rx_index = 0;
					}
				}
			}
		}

		if (!m_tx_json.empty())
		{
			send_txJson();
		}
	}

	void SerialExchange::reply_null_frame()
	{
		ESP_LOGV(TAG, "Reply null frame");
		constexpr std::array<uint8_t, 12> buffer =
			{0x5F, 0x5F, 0x5F, flag_frame, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
		m_uart.write_array(buffer.data(), buffer.size());
	}

	void SerialExchange::send_txJson()
	{
		if (m_rx_counter > 0)
		{
			// Send check counter frame
			ESP_LOGV(TAG, "Check counter %hu", m_rx_counter);
			std::array<uint8_t, 6> buffer = {
				0x5F,
				0x5F,
				0x5F,
				flag_check_counter,
				static_cast<uint8_t>(m_rx_counter >> 8),
				static_cast<uint8_t>(m_rx_counter)};
			m_uart.write_array(buffer.data(), buffer.size());
			m_rx_counter = 0;
		}

		// Build return frame
		++m_tx_counter;
		m_tx_json.push_back(0x00);

		std::string_view frame_buffer(m_tx_json.data(), m_tx_json.size());
		size_t len = frame_buffer.size();

		std::array<uint8_t, 9> frame_head{
			0x5F,
			0x5F,
			0x5F,
			flag_frame,
			static_cast<uint8_t>(m_tx_counter >> 8),
			static_cast<uint8_t>(m_tx_counter),
			static_cast<uint8_t>(len >> 16),
			static_cast<uint8_t>(len >> 8),
			static_cast<uint8_t>(len)};

		std::array<uint8_t, 2> frame_tail{checksum1(frame_buffer), checksum2(frame_buffer)};

		m_uart.write_array(frame_head);
		m_uart.write_array(reinterpret_cast<const uint8_t*>(frame_buffer.data()), frame_buffer.size());
		m_uart.write_array(frame_tail);

		m_tx_json.clear();
	}

	bool SerialExchange::rxJsonAvailable() const noexcept
	{
		return !m_rx_json.empty();
	}

	std::string SerialExchange::rxJson() noexcept
	{
		std::string rx_json = std::move(m_rx_json);
		m_rx_json.clear();
		ESP_LOGV(TAG, "RX %d: %s", rx_json.size(), rx_json.c_str());
		return rx_json;
	}

	void SerialExchange::txJson(std::string&& tx_json) noexcept
	{
		m_tx_json = std::move(tx_json);
		ESP_LOGV(TAG, "TX %d: %s", m_tx_json.size(), m_tx_json.c_str());
	}
} // namespace esphome::goodhome