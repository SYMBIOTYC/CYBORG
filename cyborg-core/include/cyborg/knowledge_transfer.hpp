#pragma once

#include <cyborg/lifecycle.hpp>
#include <cyborg/tensor.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace cyborg {

struct KnowledgePacket {
    std::string source;
    std::string type;
    std::vector<float> tensor_data;
    std::vector<size_t> shape;
    float strength;
    std::string description;
};

struct KnowledgeTransfer {
    std::string from_cyborg;
    std::string to_cyborg;
    std::vector<KnowledgePacket> packets;
    float transfer_efficiency;
    bool bidirectional;
};

class KnowledgeTransferEngine {
public:
    KnowledgeTransferEngine();
    ~KnowledgeTransferEngine();

    void registerCyborg(const std::string& name, CyborgLifecycle* lifecycle);
    bool transferKnowledge(const std::string& from, const std::string& to, const KnowledgeTransfer& transfer);
    KnowledgePacket encodeKnowledge(const std::string& description, const Tensor& tensor);
    Tensor decodeKnowledge(const KnowledgePacket& packet);
    float computeTransferEfficiency(const KnowledgeTransfer& transfer);
    std::vector<std::string> listTransfers() const;

private:
    std::unordered_map<std::string, CyborgLifecycle*> m_cyborgs;
    std::vector<KnowledgeTransfer> m_transfers;

    KnowledgePacket compressPacket(const KnowledgePacket& packet);
    KnowledgePacket decompressPacket(const KnowledgePacket& packet);
    float computeTensorSimilarity(const Tensor& a, const Tensor& b);
};

} // namespace cyborg