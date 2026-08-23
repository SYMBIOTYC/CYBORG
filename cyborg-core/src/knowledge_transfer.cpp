#include <cyborg/knowledge_transfer.hpp>
#include <cyborg/tensor_ops.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>

namespace cyborg {

KnowledgeTransferEngine::KnowledgeTransferEngine() {
}

KnowledgeTransferEngine::~KnowledgeTransferEngine() {
    m_cyborgs.clear();
    m_transfers.clear();
}

void KnowledgeTransferEngine::registerCyborg(const std::string& name, CyborgLifecycle* lifecycle) {
    m_cyborgs[name] = lifecycle;
    std::cout << "[Knowledge Transfer] Registered Cyborg: " << name << std::endl;
}

bool KnowledgeTransferEngine::transferKnowledge(const std::string& from, const std::string& to, const KnowledgeTransfer& transfer) {
    auto from_it = m_cyborgs.find(from);
    auto to_it = m_cyborgs.find(to);

    if (from_it == m_cyborgs.end() || to_it == m_cyborgs.end()) {
        std::cerr << "[Knowledge Transfer] Cyborg not registered: "
                  << (from_it == m_cyborgs.end() ? from : to) << std::endl;
        return false;
    }

    float efficiency = computeTransferEfficiency(transfer);
    if (efficiency < 0.1f) {
        std::cerr << "[Knowledge Transfer] Transfer efficiency too low: " << efficiency << std::endl;
        return false;
    }

    for (const auto& packet : transfer.packets) {
        std::cout << "[Knowledge Transfer] Transferring: " << packet.description
                  << " (strength: " << packet.strength << ")" << std::endl;
    }

    m_transfers.push_back(transfer);

    std::cout << "[Knowledge Transfer] " << from << " -> " << to
              << " | Efficiency: " << efficiency << std::endl;

    return true;
}

KnowledgePacket KnowledgeTransferEngine::encodeKnowledge(const std::string& description, const Tensor& tensor) {
    KnowledgePacket packet;
    packet.description = description;
    packet.source = "Cyborg-i1a";
    packet.type = "tensor_knowledge";
    packet.shape = tensor.shape;
    packet.strength = 1.0f;

    const auto& data = tensor.data();
    packet.tensor_data.assign(data.begin(), data.end());

    return packet;
}

Tensor KnowledgeTransferEngine::decodeKnowledge(const KnowledgePacket& packet) {
    if (packet.tensor_data.empty()) {
        return Tensor();
    }

    std::vector<size_t> shape = packet.shape.empty() ? std::vector<size_t>{packet.tensor_data.size()} : packet.shape;
    std::vector<float> data(packet.tensor_data.begin(), packet.tensor_data.end());

    return Tensor(shape, data);
}

float KnowledgeTransferEngine::computeTransferEfficiency(const KnowledgeTransfer& transfer) {
    if (transfer.packets.empty()) {
        return 0.0f;
    }

    float total_strength = 0.0f;
    for (const auto& packet : transfer.packets) {
        total_strength += packet.strength;
    }

    float avg_strength = total_strength / static_cast<float>(transfer.packets.size());
    float efficiency = avg_strength * transfer.transfer_efficiency;

    return std::clamp(efficiency, 0.0f, 1.0f);
}

std::vector<std::string> KnowledgeTransferEngine::listTransfers() const {
    std::vector<std::string> descriptions;
    for (const auto& transfer : m_transfers) {
        std::ostringstream oss;
        oss << transfer.from_cyborg << " -> " << transfer.to_cyborg
            << " | " << transfer.packets.size() << " packets"
            << " | efficiency: " << transfer.transfer_efficiency;
        descriptions.push_back(oss.str());
    }
    return descriptions;
}

} // namespace cyborg