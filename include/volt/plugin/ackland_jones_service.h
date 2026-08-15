#pragma once

#include <volt/core/volt.h>
#include <volt/core/lammps_parser.h>
#include <nlohmann/json.hpp>
#include <string>

namespace Volt {

using json = nlohmann::json;

enum class AcklandJonesStructureType : int {
    OTHER = 0,
    FCC   = 1,
    HCP   = 2,
    BCC   = 3,
    ICO   = 4,
};

inline const char* acklandJonesStructureName(AcklandJonesStructureType t) {
    switch (t) {
        case AcklandJonesStructureType::FCC:   return "FCC";
        case AcklandJonesStructureType::HCP:   return "HCP";
        case AcklandJonesStructureType::BCC:   return "BCC";
        case AcklandJonesStructureType::ICO:   return "ICO";
        default:                                return "OTHER";
    }
}

class AcklandJonesService {
public:
    json compute(const LammpsParser::Frame& frame, const std::string& outputBase);
};

}
