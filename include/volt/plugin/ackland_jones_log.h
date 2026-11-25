#pragma once

#include <volt/core/run_log.h>
#include <volt/plugin/ackland_jones_service.h>

#include <map>
#include <vector>

namespace Volt {

// The execution log of one classification: each phase hands it what it built.
class AcklandJonesLog : public RunLog {
public:
    using RunLog::dump;

    void dump(const std::vector<AcklandJonesStructureType>& types);
    // The atom count of each structure type, keyed by type.
    void dump(const std::map<int, int>& counts);
};

}
