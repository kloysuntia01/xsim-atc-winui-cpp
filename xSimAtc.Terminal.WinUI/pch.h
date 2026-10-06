#pragma once

#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>

#undef GetCurrentTime

#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <utility>

#include <rxcpp/rx.hpp>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>

#include <wil/cppwinrt_helpers.h>

#include "ViewTemplateSelector.h"

// Airfield XAML projection types
#include "AirfieldAwareViewTemplateSelector.h"


