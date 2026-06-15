#include <volt/plugin/ackland_jones_service.h>
#include <volt/analysis/nearest_neighbor_finder.h>
#include <volt/core/frame_adapter.h>
#include <volt/core/analysis_result.h>
#include <volt/plugin/output_serializer.h>
#include <spdlog/spdlog.h>

#include <cmath>

namespace Volt {

static AcklandJonesStructureType determineAJStructure(
    NearestNeighborFinder& neighFinder,
    size_t particleIndex)
{
    NearestNeighborFinder::Query<16> query(neighFinder);
    query.findNeighbors(particleIndex);

    if (query.results().size() < 6)
        return AcklandJonesStructureType::OTHER;

    // Mean squared distance of 6 nearest neighbours.
    double r0sq = 0.0;
    for (int j = 0; j < 6; ++j)
        r0sq += query.results()[j].distanceSq;
    r0sq /= 6.0;

    const double n0DistSq = 1.45 * r0sq;
    const double n1DistSq = 1.55 * r0sq;

    int n0 = 0;
    for (auto it = query.results().begin(); it != query.results().end(); ++it, ++n0)
        if (it->distanceSq > n0DistSq) break;
    auto n0end = query.results().begin() + n0;

    int n1 = n0;
    for (auto it = n0end; it != query.results().end(); ++it, ++n1)
        if (it->distanceSq >= n1DistSq) break;

    // Build the 8-bin angle histogram for the n0 shell.
    int chi[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    for (auto j = query.results().begin(); j != n0end; ++j) {
        const double normJ = std::sqrt(j->distanceSq);
        for (auto k = j + 1; k != n0end; ++k) {
            const double normK = std::sqrt(k->distanceSq);
            const double cosAngle = j->delta.dot(k->delta) / (normJ * normK);

            if      (cosAngle < -0.945)                         chi[0]++;
            else if (cosAngle < -0.915)                         chi[1]++;
            else if (cosAngle < -0.755)                         chi[2]++;
            else if (cosAngle < -0.195)                         chi[3]++;
            else if (cosAngle <  0.195)                         chi[4]++;
            else if (cosAngle <  0.245)                         chi[5]++;
            else if (cosAngle <  0.795)                         chi[6]++;
            else                                                chi[7]++;
        }
    }

    const double deltaBcc = (chi[5] + chi[6] - chi[4]) != 0
        ? 0.35 * chi[4] / static_cast<double>(chi[5] + chi[6] - chi[4])
        : 1e9;
    const double deltaCp  = std::abs(1.0 - chi[6] / 24.0);
    const double deltaFcc = 0.61 * (std::abs(chi[0] + chi[1] - 6) + chi[2]) / 6.0;
    const double deltaHcp = (std::abs(chi[0] - 3) + std::abs(chi[0] + chi[1] + chi[2] + chi[3] - 9)) / 12.0;

    double dBcc = deltaBcc, dFcc = deltaFcc, dHcp = deltaHcp;
    if      (chi[0] == 7) dBcc = 0.0;
    else if (chi[0] == 6) dFcc = 0.0;
    else if (chi[0] <= 3) dHcp = 0.0;

    if (chi[7] > 0) return AcklandJonesStructureType::OTHER;

    if (chi[4] < 3) {
        if (n1 > 13 || n1 < 11) return AcklandJonesStructureType::OTHER;
        return AcklandJonesStructureType::ICO;
    }
    if (dBcc <= deltaCp) {
        if (n1 < 11) return AcklandJonesStructureType::OTHER;
        return AcklandJonesStructureType::BCC;
    }
    if (n1 > 12 || n1 < 11) return AcklandJonesStructureType::OTHER;
    if (dFcc < dHcp)  return AcklandJonesStructureType::FCC;
    return AcklandJonesStructureType::HCP;
}

json AcklandJonesService::compute(const LammpsParser::Frame& frame, const std::string& outputBase) {
    if (frame.natoms <= 0)
        return AnalysisResult::failure("Invalid number of atoms");

    auto positions = FrameAdapter::createPositionPropertyShared(frame);
    if (!positions)
        return AnalysisResult::failure("Failed to create position property");

    NearestNeighborFinder neighFinder(14);
    if (!neighFinder.prepare(positions.get(), frame.simulationCell))
        return AnalysisResult::failure("NearestNeighborFinder::prepare failed");

    const size_t N = static_cast<size_t>(frame.natoms);
    std::vector<AcklandJonesStructureType> types(N);
    for (size_t i = 0; i < N; ++i)
        types[i] = determineAJStructure(neighFinder, i);

    std::array<int, 5> counts{};
    for (auto t : types)
        counts[static_cast<int>(t)]++;

    json result;
    result["main_listing"] = {
        {"total_atoms", frame.natoms},
        {"OTHER",       counts[0]},
        {"FCC",         counts[1]},
        {"HCP",         counts[2]},
        {"BCC",         counts[3]},
        {"ICO",         counts[4]},
    };

    if (!outputBase.empty()) {
        Plugin::serializePluginOutput(outputBase, frame, result, {
            .summaryFileSuffix  = "_ackland_jones",
            .bucketResolver     = [&types](size_t i) -> std::string {
                return acklandJonesStructureName(types[i]);
            },
            .perAtomColumnWriter = [&types](ColumnarAtomWriter& w, size_t i) {
                w.field("structure_type", static_cast<int64_t>(types[i]));
            },
        });
    }

    return result;
}

} // namespace Volt
