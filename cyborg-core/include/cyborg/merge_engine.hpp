#pragma once

#include <cyborg/tensor.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace cyborg {

class MergeEngine {
public:
    enum class Strategy {
        Average,
        Superposition,
        Interleave
    };

    MergeEngine();
    ~MergeEngine();

    void merge(const std::vector<Tensor>& models, const std::string& strategy);
    void save(const std::string& output_path);

private:
    std::vector<Tensor> merged_tensors_;

    static Strategy parse_strategy(const std::string& strategy);
    static void validate_compatibility(const std::vector<Tensor>& models);

    void merge_average(const std::vector<Tensor>& models);
    void merge_superposition(const std::vector<Tensor>& models);
    void merge_interleave(const std::vector<Tensor>& models);
    void write_gguf(const std::string& path);
};

}