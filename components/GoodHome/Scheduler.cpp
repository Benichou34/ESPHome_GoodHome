#include "Scheduler.h"

/*
"value":{
    "0":[
        {
            "s":"0",
            "e":"420"
        },
        {
            "s":"1260",
            "e":"1440"
        }
    ],
    "1":[
        {
            "s":"0",
            "e":"420"
        },
        {
            "s":"1260",
            "e":"1440"
        }
    ],
    "2":[
        {
            "s":"0",
            "e":"420"
        },
        {
            "s":"1260",
            "e":"1440"
        }
    ],
    "3":[
        {
            "s":"0",
            "e":"420"
        },
        {
            "s":"1260",
            "e":"1440"
        }
    ],
    "4":[
        {
            "s":"0",
            "e":"420"
        },
        {
            "s":"1260",
            "e":"1440"
        }
    ],
    "5":[
        {
            "s":"0",
            "e":"540"
        },
        {
            "s":"1260",
            "e":"1440"
        }
    ],
    "6":[
        {
            "s":"0",
            "e":"540"
        },
        {
            "s":"1260",
            "e":"1440"
        }
    ]
}
*/
namespace esphome::goodhome
{
	Scheduler::Scheduler()
	{}

	void Scheduler::fromJson(const JsonObject& js) noexcept
	{
		for (size_t i = 0; i < m_week.size(); ++i)
		{
			std::string key = to_string(i);
			if (js[key].is<JsonArrayConst>())
			{
				JsonArrayConst jsDay = js[key].as<JsonArrayConst>();
				for (size_t j = 0; j < std::min<size_t>(jsDay.size(), 4); ++j)
				{
					m_week[i][j].first =
						std::chrono::minutes(parse_number<size_t>(jsDay[j]["s"].as<std::string>()).value_or(0));
					m_week[i][j].second =
						std::chrono::minutes(parse_number<size_t>(jsDay[j]["e"].as<std::string>()).value_or(0));
				}
			}
		}
	}

	void Scheduler::toJson(JsonObject& js) const noexcept
	{
		for (size_t i = 0; i < m_week.size(); ++i)
		{
			JsonArray jsDay = js[to_string(i)].add<JsonArray>();
			for (const auto& period : m_week[i])
			{
				if (period.first > std::chrono::minutes::zero() || period.second > std::chrono::minutes::zero())
				{
					JsonObject jsPeriod = jsDay.add<JsonObject>();
					jsPeriod["s"] = to_string(period.first.count());
					jsPeriod["e"] = to_string(period.second.count());
				}
			}
		}
	}
} // namespace esphome::goodhome