#include "pch.h"
#include "aircraft_base.h"

#include <objbase.h>
#include <rxcpp/rx.hpp>
#include <stdexcept>
#include <utility>

namespace xsim::aircrafts
{
    using std::move;
    using std::runtime_error;
    using std::string;
    
	
	void rxcpp_probe() {
		auto values = rxcpp::observable<>::just(42);

		values.subscribe([](int value) {
			(void)value;
			});
	}
			
    
}
