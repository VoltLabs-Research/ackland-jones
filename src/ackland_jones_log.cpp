#include <volt/plugin/ackland_jones_log.h>
#include <volt/core/type_counts.h>
#include <spdlog/spdlog.h>

#include <utility>

namespace Volt {

void AcklandJonesLog::dump(const std::vector<AcklandJonesStructureType>& types) {
    spdlog::info("Classified {} atoms in {}", types.size(), phaseElapsed());
}

void AcklandJonesLog::dump(const std::map<int, int>& counts) {
    static const std::pair<AcklandJonesStructureType, const char*> listed[] = {
        {AcklandJonesStructureType::FCC, "FCC"},
        {AcklandJonesStructureType::HCP, "HCP"},
        {AcklandJonesStructureType::BCC, "BCC"},
        {AcklandJonesStructureType::ICO, "ICO"},
        {AcklandJonesStructureType::OTHER, "Other"},
    };
    TypeCounts line{"Structure types"};
    for (const auto& [type, name] : listed) {
        const auto count = counts.find(static_cast<int>(type));
        line.counts.emplace_back(name, count != counts.end() ? count->second : 0);
        line.atoms += line.counts.back().second;
    }
    dump(line);
}

}
