#pragma once

#include <chrono>
#include <guiddef.h>
#include <string>

namespace xsim::aircrafts
{
	struct TransponderSignal final
	{
		GUID aircraft_id{};
		std::string call_sign;

		std::chrono::steady_clock::time_point timestamp{};
	};
}
