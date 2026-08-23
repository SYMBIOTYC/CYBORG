#include <cyborg/merge_engine.hpp>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <numeric>
#include <stdexcept>

namespace cyborg {

MergeEngine::MergeEngine() = default;

MergeEngine::~MergeEngine() = default;

void MergeEngine::merge(const std::vector<Tensor>& models, const std::string& strategy) {
    if (models.empty()) {
        throw std::invalid_argument("No models provided for merge");
    }
    validate_compatibility(models);
    Strategy s = parse_strategy(strategy);
    switch (s) {
        case Strategy::Average:
            merge_average(models);
            break;
        case Strategy::Superposition:
            merge_superposition(models);
            break;
        case Strategy::Interleave:
            merge_interleave(models);
            break;
    }
}

void MergeEngine::save(const std::string& output_path) {
    if (merged_tensors_.empty()) {
        throw std::runtime_error("No merged tensors to save");
    }
    write_gguf(output_path);
}

MergeEngine::Strategy MergeEngine::parse_strategy(const std::string& strategy) {
    if (strategy == "average") {
        return Strategy::Average;
    }
    if (strategy == "superposition") {
        return Strategy::Superposition;
    }
    if (strategy == "interleave") {
        return Strategy::Interleave;
    }
    throw std::invalid_argument("Unknown merge strategy: " + strategy);
}

void MergeEngine::validate_compatibility(const std::vector<Tensor>& models) {
    if (models.empty()) {
        return;
    }
    const Tensor& first = models[0];
    for (size_t i = 1; i < models.size(); ++i) {
        if (first.shape != models[i].shape || first.name != models[i].name) {
            throw std::invalid_argument(
                "Tensor compatibility check failed: '" + first.name +
                "' vs '" + models[i].name + "'");
        }
    }
}

void MergeEngine::merge_average(const std::vector<Tensor>& models) {
    const Tensor& first = models[0];
    Tensor result;
    result.name = first.name;
    result.shape = first.shape;
    size_t n = first.numElements();
    result.data().resize(n, 0.0f);

    for (const auto& model : models) {
        for (size_t i = 0; i < n; ++i) {
            result.data()[i] += model.data()[i];
        }
    }

    float scale = 1.0f / static_cast<float>(models.size());
    for (size_t i = 0; i < n; ++i) {
        result.data()[i] *= scale;
    }

    merged_tensors_.push_back(std::move(result));
}

void MergeEngine::merge_superposition(const std::vector<Tensor>& models) {
    const Tensor& first = models[0];
    Tensor result;
    result.name = first.name;
    result.shape = first.shape;
    size_t n = first.numElements();
    result.data().resize(n, 0.0f);

    for (const auto& model : models) {
        for (size_t i = 0; i < n; ++i) {
            result.data()[i] += model.data()[i];
        }
    }

    merged_tensors_.push_back(std::move(result));
}

void MergeEngine::merge_interleave(const std::vector<Tensor>& models) {
    const Tensor& first = models[0];
    Tensor result;
    result.name = first.name;
    result.shape = first.shape;
    size_t n = first.numElements();
    result.data().resize(n);

    size_t num_models = models.size();
    for (size_t i = 0; i < n; ++i) {
        size_t model_idx = i % num_models;
        result.data()[i] = models[model_idx].data()[i];
    }

    merged_tensors_.push_back(std::move(result));
}

void MergeEngine::write_gguf(const std::string& path) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open output file: " + path);
    }

    file.write("GGUF", 4);

    uint32_t version = 3;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));

    uint64_t tensor_count = static_cast<uint64_t>(merged_tensors_.size());
    file.write(reinterpret_cast<const char*>(&tensor_count), sizeof(tensor_count));

    uint64_t metadata_count = 0;
    file.write(reinterpret_cast<const char*>(&metadata_count), sizeof(metadata_count));

    size_t data_offset = 0;
    size_t alignment = 32;

    for (const auto& tensor : merged_tensors_) {
        uint64_t name_len = static_cast<uint64_t>(tensor.name.size());
        file.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        file.write(tensor.name.c_str(), name_len);
        size_t name_padding = (alignment - (name_len % alignment)) % alignment;
        for (size_t p = 0; p < name_padding; ++p) {
            char pad = 0;
            file.write(&pad, 1);
        }

        uint32_t n_dims = static_cast<uint32_t>(tensor.shape.size());
        file.write(reinterpret_cast<const char*>(&n_dims), sizeof(n_dims));

        for (auto dim : tensor.shape) {
            uint64_t dim_val = static_cast<uint64_t>(dim);
            file.write(reinterpret_cast<const char*>(&dim_val), sizeof(dim_val));
        }

        uint32_t gguf_type = 0;
        file.write(reinterpret_cast<const char*>(&gguf_type), sizeof(gguf_type));

        uint64_t offset_val = static_cast<uint64_t>(data_offset);
        file.write(reinterpret_cast<const char*>(&offset_val), sizeof(offset_val));

        data_offset += tensor.data().size() * sizeof(float);
    }

    size_t current_pos = static_cast<size_t>(file.tellp());
    size_t padding_bytes = (alignment - (current_pos % alignment)) % alignment;
    for (size_t p = 0; p < padding_bytes; ++p) {
        char pad = 0;
        file.write(&pad, 1);
    }

    for (const auto& tensor : merged_tensors_) {
        file.write(reinterpret_cast<const char*>(tensor.data().data()),
                   tensor.data().size() * sizeof(float));
    }

    file.close();
}

}