#pragma once

#include <cyborg/gguf_types.hpp>
#include <cyborg/tensor.hpp>

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace cyborg {

struct TensorMetadata {
    std::string name;
    std::vector<size_t> shape;
    GGUFTensorType dtype;
    size_t data_size;
};

class ModelLoader {
public:
    ModelLoader();
    ~ModelLoader();

    ModelLoader(const ModelLoader&) = delete;
    ModelLoader& operator=(const ModelLoader&) = delete;
    ModelLoader(ModelLoader&&) = delete;
    ModelLoader& operator=(ModelLoader&&) = delete;

    bool load(const std::string& path);
    Tensor* getTensor(const std::string& name);
    const Tensor* getTensor(const std::string& name) const;
    std::vector<TensorMetadata> listTensors() const;
    void unload();

private:
    const uint8_t* m_data;
    size_t m_size;

    std::vector<Tensor> m_tensors;
    std::unordered_map<std::string, size_t> m_tensorMap;
    std::vector<TensorMetadata> m_tensorMetadata;
    std::unordered_map<std::string, std::string> m_metadata;

    static uint16_t readUInt16(const uint8_t* ptr);
    static uint32_t readUInt32(const uint8_t* ptr);
    static uint64_t readUInt64(const uint8_t* ptr);
    static float readFloat32(const uint8_t* ptr);
    static double readFloat64(const uint8_t* ptr);

    static float float16ToFloat32(uint16_t h);

    static void dequantizeF32(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeF16(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ4_0(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ4_1(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ5_0(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ5_1(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ8_0(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ8_1(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ4_K(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ5_K(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ6_K(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ2_K(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ3_K(const uint8_t* data, size_t numElements, std::vector<float>& output);
    static void dequantizeQ8_K(const uint8_t* data, size_t numElements, std::vector<float>& output);

    static bool dequantizeTensor(const uint8_t* data, size_t numElements, GGUFTensorType type, std::vector<float>& output);
};

}