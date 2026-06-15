#include <volt/plugin/plugin_entry.h>
#include <volt/plugin/ackland_jones_service.h>

using namespace Volt;
using namespace Volt::Plugin;
using S = AcklandJonesService;

static const std::vector<OptionBinding<S>> bindings = {};

VOLT_SERVICE_PLUGIN("volt-ackland-jones", "Ackland-Jones Structure Classifier", S, bindings)
