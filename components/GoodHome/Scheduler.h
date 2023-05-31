#pragma once
#include "esphome/components/json/json_util.h"
#include <chrono>

namespace esphome::goodhome
{
	class Scheduler
	{
	public:
		Scheduler();
		~Scheduler() = default;

		void fromJson(const JsonObject& js) noexcept;
		void toJson(JsonObject& js) const noexcept;

	private:
		// Scheduler of comfort periods (in minutes)
		std::array<std::array<std::pair<std::chrono::minutes, std::chrono::minutes>, 4>, 7> m_week;
	};
} // namespace esphome::goodhome