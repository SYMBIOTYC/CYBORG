#include <cyborg/model_loader.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace cyborg {

ModelLoader::ModelLoader()
    : m_data(nullptr)
    , m_size(0) {
}

ModelLoader::~ModelLoader() {
    unload();
}

void ModelLoader::unload() {
    if (m_data != nullptr) {
        munmap(const_cast<uint8_t*>(m_data), m_size);
        m_data = nullptr;
        m_size = 0;
    }
    m_tensors.clear();
    m_tensorMap.clear();
    m_tensorMetadata.clear();
    m_metadata.clear();
}

bool ModelLoader::load(const std::string& path) {
    unload();

    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        return false;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return false;
    }
    m_size = static_cast<size_t>(st.st_size);

    if (m_size < 24) {
        close(fd);
        return false;
    }

    m_data = static_cast<const uint8_t*>(mmap(nullptr, m_size, PROT_READ, MAP_PRIVATE, fd, 0));
    close(fd);
    if (m_data == MAP_FAILED) {
        m_data = nullptr;
        m_size = 0;
        return false;
    }

    uint32_t magic = readUInt32(m_data);
    if (magic != GGUF_MAGIC) {
        unload();
        return false;
    }

    uint32_t version = readUInt32(m_data + 4);
    if (version != 3) {
        unload();
        return false;
    }

    uint64_t tensorCount = readUInt64(m_data + 8);
    uint64_t metadataKeyCount = readUInt64(m_data + 16);

    size_t offset = 24;

    for (uint64_t i = 0; i < metadataKeyCount; ++i) {
        if (offset + 8 > m_size) {
            unload();
            return false;
        }
        uint64_t keyLen = readUInt64(m_data + offset);
        offset += 8;
        if (offset + keyLen > m_size) {
            unload();
            return false;
        }
        std::string key(reinterpret_cast<const char*>(m_data + offset), keyLen);
        offset += keyLen;
        if (offset + 4 > m_size) {
            unload();
            return false;
        }
        uint32_t valueType = readUInt32(m_data + offset);
        offset += 4;

        switch (static_cast<GGUFValueType>(valueType)) {
            case GGUFValueType::GGUF_TYPE_UINT8: {
                if (offset + 1 > m_size) { unload(); return false; }
                m_metadata[key] = std::to_string(m_data[offset]);
                offset += 1;
                break;
            }
            case GGUFValueType::GGUF_TYPE_INT8: {
                if (offset + 1 > m_size) { unload(); return false; }
                m_metadata[key] = std::to_string(static_cast<int8_t>(m_data[offset]));
                offset += 1;
                break;
            }
            case GGUFValueType::GGUF_TYPE_UINT16: {
                if (offset + 2 > m_size) { unload(); return false; }
                m_metadata[key] = std::to_string(readUInt16(m_data + offset));
                offset += 2;
                break;
            }
            case GGUFValueType::GGUF_TYPE_INT16: {
                if (offset + 2 > m_size) { unload(); return false; }
                m_metadata[key] = std::to_string(static_cast<int16_t>(readUInt16(m_data + offset)));
                offset += 2;
                break;
            }
            case GGUFValueType::GGUF_TYPE_UINT32: {
                if (offset + 4 > m_size) { unload(); return false; }
                m_metadata[key] = std::to_string(readUInt32(m_data + offset));
                offset += 4;
                break;
            }
            case GGUFValueType::GGUF_TYPE_INT32: {
                if (offset + 4 > m_size) { unload(); return false; }
                m_metadata[key] = std::to_string(static_cast<int32_t>(readUInt32(m_data + offset)));
                offset += 4;
                break;
            }
            case GGUFValueType::GGUF_TYPE_FLOAT32: {
                if (offset + 4 > m_size) { unload(); return false; }
                m_metadata[key] = std::to_string(readFloat32(m_data + offset));
                offset += 4;
                break;
            }
            case GGUFValueType::GGUF_TYPE_DOUBLE: {
                if (offset + 8 > m_size) { unload(); return false; }
                m_metadata[key] = std::to_string(readFloat64(m_data + offset));
                offset += 8;
                break;
            }
            case GGUFValueType::GGUF_TYPE_UINT64: {
                if (offset + 8 > m_size) { unload(); return false; }
                m_metadata[key] = std::to_string(readUInt64(m_data + offset));
                offset += 8;
                break;
            }
            case GGUFValueType::GGUF_TYPE_INT64: {
                if (offset + 8 > m_size) { unload(); return false; }
                m_metadata[key] = std::to_string(static_cast<int64_t>(readUInt64(m_data + offset)));
                offset += 8;
                break;
            }
            case GGUFValueType::GGUF_TYPE_BOOL: {
                if (offset + 1 > m_size) { unload(); return false; }
                m_metadata[key] = m_data[offset] ? "true" : "false";
                offset += 1;
                break;
            }
            case GGUFValueType::GGUF_TYPE_STRING: {
                if (offset + 8 > m_size) { unload(); return false; }
                uint64_t strLen = readUInt64(m_data + offset);
                offset += 8;
                if (offset + strLen > m_size) { unload(); return false; }
                m_metadata[key] = std::string(reinterpret_cast<const char*>(m_data + offset), strLen);
                offset += strLen;
                break;
            }
            case GGUFValueType::GGUF_TYPE_ARRAY: {
                if (offset + 4 > m_size) { unload(); return false; }
                uint32_t arrayType = readUInt32(m_data + offset);
                offset += 4;
                if (offset + 8 > m_size) { unload(); return false; }
                uint64_t arrayCount = readUInt64(m_data + offset);
                offset += 8;
                size_t elemSize = 0;
                switch (static_cast<GGUFValueType>(arrayType)) {
                    case GGUFValueType::GGUF_TYPE_UINT8: elemSize = 1; break;
                    case GGUFValueType::GGUF_TYPE_INT8: elemSize = 1; break;
                    case GGUFValueType::GGUF_TYPE_UINT16: elemSize = 2; break;
                    case GGUFValueType::GGUF_TYPE_INT16: elemSize = 2; break;
                    case GGUFValueType::GGUF_TYPE_UINT32: elemSize = 4; break;
                    case GGUFValueType::GGUF_TYPE_INT32: elemSize = 4; break;
                    case GGUFValueType::GGUF_TYPE_FLOAT32: elemSize = 4; break;
                    case GGUFValueType::GGUF_TYPE_DOUBLE: elemSize = 8; break;
                    case GGUFValueType::GGUF_TYPE_UINT64: elemSize = 8; break;
                    case GGUFValueType::GGUF_TYPE_INT64: elemSize = 8; break;
                    case GGUFValueType::GGUF_TYPE_BOOL: elemSize = 1; break;
                    default: elemSize = 0; break;
                }
                offset += elemSize * arrayCount;
                m_metadata[key] = "array";
                break;
            }
            default:
                unload();
                return false;
        }
    }

    for (uint64_t i = 0; i < tensorCount; ++i) {
        if (offset + 8 > m_size) {
            unload();
            return false;
        }
        uint64_t nameLen = readUInt64(m_data + offset);
        offset += 8;
        if (offset + nameLen > m_size) {
            unload();
            return false;
        }
        std::string tensorName(reinterpret_cast<const char*>(m_data + offset), nameLen);
        offset += nameLen;

        if (offset + 4 > m_size) {
            unload();
            return false;
        }
        uint32_t nDims = readUInt32(m_data + offset);
        offset += 4;

        if (offset + nDims * 8 > m_size) {
            unload();
            return false;
        }
        std::vector<size_t> shape;
        shape.reserve(nDims);
        for (uint32_t d = 0; d < nDims; ++d) {
            uint64_t dim = readUInt64(m_data + offset);
            offset += 8;
            shape.push_back(static_cast<size_t>(dim));
        }

        if (offset + 4 > m_size) {
            unload();
            return false;
        }
        uint32_t tensorTypeVal = readUInt32(m_data + offset);
        offset += 4;
        GGUFTensorType tensorType = static_cast<GGUFTensorType>(tensorTypeVal);

        if (offset + 8 > m_size) {
            unload();
            return false;
        }
        uint64_t tensorOffset = readUInt64(m_data + offset);
        offset += 8;

        size_t numElements = 1;
        for (auto dim : shape) {
            numElements *= dim;
        }

        size_t nextOffset = m_size;
        if (i + 1 < tensorCount) {
            size_t nextTensorStart = offset;
            if (nextTensorStart + 8 > m_size) {
                unload();
                return false;
            }
            uint64_t nextNameLen = readUInt64(m_data + nextTensorStart);
            nextTensorStart += 8 + nextNameLen + 4;
            for (uint32_t d = 0; d < nDims; ++d) {
                nextTensorStart += 8;
            }
            nextTensorStart += 4 + 8;
            nextOffset = nextTensorStart;
        }

        size_t dataSize = 0;
        if (tensorOffset < m_size) {
            dataSize = (nextOffset > tensorOffset) ? (nextOffset - tensorOffset) : 0;
        }

        TensorMetadata meta;
        meta.name = tensorName;
        meta.shape = shape;
        meta.dtype = tensorType;
        meta.data_size = dataSize;
        m_tensorMetadata.push_back(meta);

        Tensor tensor;
        tensor.name = tensorName;
        tensor.shape = shape;

        const uint8_t* tensorData = m_data + tensorOffset;
        std::vector<float> dequantized;
        if (!dequantizeTensor(tensorData, numElements, tensorType, dequantized)) {
            unload();
            return false;
        }
        tensor.data = std::move(dequantized);

        m_tensors.push_back(std::move(tensor));
        m_tensorMap[tensorName] = m_tensors.size() - 1;
    }

    return true;
}

Tensor* ModelLoader::getTensor(const std::string& name) {
    auto it = m_tensorMap.find(name);
    if (it == m_tensorMap.end()) {
        return nullptr;
    }
    return &m_tensors[it->second];
}

const Tensor* ModelLoader::getTensor(const std::string& name) const {
    auto it = m_tensorMap.find(name);
    if (it == m_tensorMap.end()) {
        return nullptr;
    }
    return &m_tensors[it->second];
}

std::vector<TensorMetadata> ModelLoader::listTensors() const {
    return m_tensorMetadata;
}

uint16_t ModelLoader::readUInt16(const uint8_t* ptr) {
    uint16_t value;
    std::memcpy(&value, ptr, sizeof(value));
    return value;
}

uint32_t ModelLoader::readUInt32(const uint8_t* ptr) {
    uint32_t value;
    std::memcpy(&value, ptr, sizeof(value));
    return value;
}

uint64_t ModelLoader::readUInt64(const uint8_t* ptr) {
    uint64_t value;
    std::memcpy(&value, ptr, sizeof(value));
    return value;
}

float ModelLoader::readFloat32(const uint8_t* ptr) {
    float value;
    std::memcpy(&value, ptr, sizeof(value));
    return value;
}

double ModelLoader::readFloat64(const uint8_t* ptr) {
    double value;
    std::memcpy(&value, ptr, sizeof(value));
    return value;
}

float ModelLoader::float16ToFloat32(uint16_t h) {
    uint32_t sign = (h >> 15) & 1;
    int32_t exp = (h >> 10) & 0x1F;
    uint32_t mantissa = h & 0x3FF;

    if (exp == 0) {
        if (mantissa == 0) {
            uint32_t u = sign << 31;
            float f;
            std::memcpy(&f, &u, sizeof(f));
            return f;
        }
        exp = 1;
        while ((mantissa & 0x400) == 0) {
            mantissa <<= 1;
            exp--;
        }
        mantissa &= 0x3FF;
    }

    if (exp == 31) {
        uint32_t u = (sign << 31) | (0xFF << 23) | (mantissa << 13);
        float f;
        std::memcpy(&f, &u, sizeof(f));
        return f;
    }

    uint32_t u = (sign << 31) | ((exp + (127 - 15)) << 23) | (mantissa << 13);
    float f;
    std::memcpy(&f, &u, sizeof(f));
    return f;
}

void ModelLoader::dequantizeF32(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    output.resize(numElements);
    std::memcpy(output.data(), data, numElements * sizeof(float));
}

void ModelLoader::dequantizeF16(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    output.resize(numElements);
    const uint16_t* src = reinterpret_cast<const uint16_t*>(data);
    for (size_t i = 0; i < numElements; ++i) {
        output[i] = float16ToFloat32(src[i]);
    }
}

void ModelLoader::dequantizeQ4_0(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t blockSize = 32;
    const size_t numBlocks = (numElements + blockSize - 1) / blockSize;
    output.resize(numElements);

    for (size_t block = 0; block < numBlocks; ++block) {
        const uint8_t* blockPtr = data + block * (blockSize / 2 + 4);
        float scale;
        std::memcpy(&scale, blockPtr + blockSize / 2, sizeof(float));
        float min;
        std::memcpy(&min, blockPtr + blockSize / 2 + 4, sizeof(float));

        for (size_t i = 0; i < blockSize / 2; ++i) {
            uint8_t byte = blockPtr[i];
            int8_t lo = static_cast<int8_t>(byte & 0x0F);
            int8_t hi = static_cast<int8_t>((byte >> 4) & 0x0F);
            size_t idx0 = block * blockSize + i * 2;
            size_t idx1 = block * blockSize + i * 2 + 1;
            if (idx0 < numElements) output[idx0] = scale * (lo - min);
            if (idx1 < numElements) output[idx1] = scale * (hi - min);
        }
    }
}

void ModelLoader::dequantizeQ4_1(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t blockSize = 32;
    const size_t numBlocks = (numElements + blockSize - 1) / blockSize;
    output.resize(numElements);

    for (size_t block = 0; block < numBlocks; ++block) {
        const uint8_t* blockPtr = data + block * (blockSize / 2 + 8);
        float scale;
        std::memcpy(&scale, blockPtr + blockSize / 2, sizeof(float));
        float min;
        std::memcpy(&min, blockPtr + blockSize / 2 + 4, sizeof(float));

        for (size_t i = 0; i < blockSize / 2; ++i) {
            uint8_t byte = blockPtr[i];
            int8_t lo = static_cast<int8_t>(byte & 0x0F);
            int8_t hi = static_cast<int8_t>((byte >> 4) & 0x0F);
            size_t idx0 = block * blockSize + i * 2;
            size_t idx1 = block * blockSize + i * 2 + 1;
            if (idx0 < numElements) output[idx0] = scale * (lo - min);
            if (idx1 < numElements) output[idx1] = scale * (hi - min);
        }
    }
}

void ModelLoader::dequantizeQ5_0(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t blockSize = 32;
    const size_t numBlocks = (numElements + blockSize - 1) / blockSize;
    output.resize(numElements);

    for (size_t block = 0; block < numBlocks; ++block) {
        const uint8_t* blockPtr = data + block * (blockSize / 2 + 4 + 4);
        float scale;
        std::memcpy(&scale, blockPtr + blockSize / 2, sizeof(float));
        float min;
        std::memcpy(&min, blockPtr + blockSize / 2 + 4, sizeof(float));
        const uint8_t* qh = blockPtr + blockSize / 2 + 8;

        for (size_t i = 0; i < blockSize / 2; ++i) {
            uint8_t byte = blockPtr[i];
            uint8_t qhBit = (qh[i / 8] >> (i % 8)) & 1;
            int8_t lo = static_cast<int8_t>((byte & 0x0F) | (qhBit << 4));
            int8_t hi = static_cast<int8_t>(((byte >> 4) & 0x0F) | (qhBit << 4));
            size_t idx0 = block * blockSize + i * 2;
            size_t idx1 = block * blockSize + i * 2 + 1;
            if (idx0 < numElements) output[idx0] = scale * (lo - min);
            if (idx1 < numElements) output[idx1] = scale * (hi - min);
        }
    }
}

void ModelLoader::dequantizeQ5_1(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t blockSize = 32;
    const size_t numBlocks = (numElements + blockSize - 1) / blockSize;
    output.resize(numElements);

    for (size_t block = 0; block < numBlocks; ++block) {
        const uint8_t* blockPtr = data + block * (blockSize / 2 + 4 + 4 + 4);
        float scale;
        std::memcpy(&scale, blockPtr + blockSize / 2, sizeof(float));
        float min;
        std::memcpy(&min, blockPtr + blockSize / 2 + 4, sizeof(float));
        const uint8_t* qh = blockPtr + blockSize / 2 + 8;

        for (size_t i = 0; i < blockSize / 2; ++i) {
            uint8_t byte = blockPtr[i];
            uint8_t qhBit = (qh[i / 8] >> (i % 8)) & 1;
            int8_t lo = static_cast<int8_t>((byte & 0x0F) | (qhBit << 4));
            int8_t hi = static_cast<int8_t>(((byte >> 4) & 0x0F) | (qhBit << 4));
            size_t idx0 = block * blockSize + i * 2;
            size_t idx1 = block * blockSize + i * 2 + 1;
            if (idx0 < numElements) output[idx0] = scale * (lo - min);
            if (idx1 < numElements) output[idx1] = scale * (hi - min);
        }
    }
}

void ModelLoader::dequantizeQ8_0(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t blockSize = 32;
    const size_t numBlocks = (numElements + blockSize - 1) / blockSize;
    output.resize(numElements);

    for (size_t block = 0; block < numBlocks; ++block) {
        const uint8_t* blockPtr = data + block * (blockSize + 4);
        float scale;
        std::memcpy(&scale, blockPtr + blockSize, sizeof(float));

        for (size_t i = 0; i < blockSize && (block * blockSize + i) < numElements; ++i) {
            output[block * blockSize + i] = scale * static_cast<int8_t>(blockPtr[i]);
        }
    }
}

void ModelLoader::dequantizeQ8_1(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t blockSize = 32;
    const size_t numBlocks = (numElements + blockSize - 1) / blockSize;
    output.resize(numElements);

    for (size_t block = 0; block < numBlocks; ++block) {
        const uint8_t* blockPtr = data + block * (blockSize + 8);
        float scale;
        std::memcpy(&scale, blockPtr + blockSize, sizeof(float));
        float min;
        std::memcpy(&min, blockPtr + blockSize + 4, sizeof(float));

        for (size_t i = 0; i < blockSize && (block * blockSize + i) < numElements; ++i) {
            output[block * blockSize + i] = scale * (static_cast<int8_t>(blockPtr[i]) - min);
        }
    }
}

void ModelLoader::dequantizeQ4_K(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t superBlockSize = 256;
    const size_t subBlockSize = 32;
    const size_t numSuperBlocks = (numElements + superBlockSize - 1) / superBlockSize;
    output.resize(numElements);

    for (size_t superBlock = 0; superBlock < numSuperBlocks; ++superBlock) {
        const uint8_t* superBlockPtr = data + superBlock * 152;
        const uint8_t* scales = superBlockPtr + 128;
        const uint8_t* mins = scales + 8;
        const uint8_t* qh = mins + 8;
        const uint8_t* qData = qh + 8;

        for (size_t subBlock = 0; subBlock < 8; ++subBlock) {
            const uint8_t* subBlockPtr = qData + superBlock * 128 + subBlock * 16;
            float scale = static_cast<float>(static_cast<int8_t>(scales[superBlock * 8 + subBlock]));
            float min = static_cast<float>(static_cast<int8_t>(mins[superBlock * 8 + subBlock]));

            for (size_t i = 0; i < subBlockSize / 2; ++i) {
                uint8_t byte = subBlockPtr[i];
                int8_t lo = static_cast<int8_t>(byte & 0x0F);
                int8_t hi = static_cast<int8_t>((byte >> 4) & 0x0F);
                size_t idx0 = superBlock * superBlockSize + subBlock * subBlockSize + i * 2;
                size_t idx1 = superBlock * superBlockSize + subBlock * subBlockSize + i * 2 + 1;
                if (idx0 < numElements) output[idx0] = scale * (lo - min);
                if (idx1 < numElements) output[idx1] = scale * (hi - min);
            }
        }
    }
}

void ModelLoader::dequantizeQ5_K(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t superBlockSize = 256;
    const size_t subBlockSize = 32;
    const size_t numSuperBlocks = (numElements + superBlockSize - 1) / superBlockSize;
    output.resize(numElements);

    for (size_t superBlock = 0; superBlock < numSuperBlocks; ++superBlock) {
        const uint8_t* superBlockPtr = data + superBlock * 176;
        const uint8_t* scales = superBlockPtr + 128;
        const uint8_t* mins = scales + 8;
        const uint8_t* qh = mins + 8;
        const uint8_t* qData = qh + 32;

        for (size_t subBlock = 0; subBlock < 8; ++subBlock) {
            const uint8_t* subBlockPtr = qData + superBlock * 128 + subBlock * 16;
            float scale = static_cast<float>(static_cast<int8_t>(scales[superBlock * 8 + subBlock]));
            float min = static_cast<float>(static_cast<int8_t>(mins[superBlock * 8 + subBlock]));

            for (size_t i = 0; i < subBlockSize / 2; ++i) {
                uint8_t byte = subBlockPtr[i];
                int8_t lo = static_cast<int8_t>(byte & 0x0F);
                int8_t hi = static_cast<int8_t>((byte >> 4) & 0x0F);
                size_t idx0 = superBlock * superBlockSize + subBlock * subBlockSize + i * 2;
                size_t idx1 = superBlock * superBlockSize + subBlock * subBlockSize + i * 2 + 1;
                uint8_t qhBit0 = (qh[idx0 / 8] >> (idx0 % 8)) & 1;
                uint8_t qhBit1 = (qh[idx1 / 8] >> (idx1 % 8)) & 1;
                int8_t fullLo = static_cast<int8_t>((lo & 0x0F) | (qhBit0 << 4));
                int8_t fullHi = static_cast<int8_t>((hi & 0x0F) | (qhBit1 << 4));
                if (idx0 < numElements) output[idx0] = scale * (fullLo - min);
                if (idx1 < numElements) output[idx1] = scale * (fullHi - min);
            }
        }
    }
}

void ModelLoader::dequantizeQ6_K(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t superBlockSize = 256;
    const size_t subBlockSize = 32;
    const size_t numSuperBlocks = (numElements + superBlockSize - 1) / superBlockSize;
    output.resize(numElements);

    for (size_t superBlock = 0; superBlock < numSuperBlocks; ++superBlock) {
        const uint8_t* superBlockPtr = data + superBlock * 168;
        const uint8_t* scales = superBlockPtr + 128;
        const uint8_t* qh = scales + 8;
        const uint8_t* qData = qh + 32;

        for (size_t subBlock = 0; subBlock < 8; ++subBlock) {
            const uint8_t* subBlockPtr = qData + superBlock * 128 + subBlock * 16;
            float scale = static_cast<float>(static_cast<int8_t>(scales[superBlock * 8 + subBlock]));

            for (size_t i = 0; i < subBlockSize / 2; ++i) {
                uint8_t byte = subBlockPtr[i];
                size_t elemIdx0 = superBlock * superBlockSize + subBlock * subBlockSize + i * 2;
                size_t elemIdx1 = elemIdx0 + 1;
                uint8_t qhBits0 = (qh[elemIdx0 / 4] >> ((elemIdx0 % 4) * 2)) & 0x03;
                uint8_t qhBits1 = (qh[elemIdx1 / 4] >> ((elemIdx1 % 4) * 2)) & 0x03;
                int8_t lo = static_cast<int8_t>((byte & 0x0F) | (qhBits0 << 4));
                int8_t hi = static_cast<int8_t>(((byte >> 4) & 0x0F) | (qhBits1 << 4));
                if (elemIdx0 < numElements) output[elemIdx0] = scale * lo;
                if (elemIdx1 < numElements) output[elemIdx1] = scale * hi;
            }
        }
    }
}

void ModelLoader::dequantizeQ2_K(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t superBlockSize = 256;
    const size_t subBlockSize = 32;
    const size_t numSuperBlocks = (numElements + superBlockSize - 1) / superBlockSize;
    output.resize(numElements);

    for (size_t superBlock = 0; superBlock < numSuperBlocks; ++superBlock) {
        const uint8_t* superBlockPtr = data + superBlock * 140;
        const uint8_t* scales = superBlockPtr + 64;
        const uint8_t* qh = scales + 8;
        const uint8_t* qData = qh + 4;

        for (size_t subBlock = 0; subBlock < 4; ++subBlock) {
            const uint8_t* subBlockPtr = qData + superBlock * 64 + subBlock * 8;
            float scale = static_cast<float>(static_cast<int8_t>(scales[superBlock * 4 + subBlock]));

            for (size_t i = 0; i < subBlockSize / 4; ++i) {
                uint8_t byte = subBlockPtr[i];
                int8_t q0 = static_cast<int8_t>(byte & 0x03);
                int8_t q1 = static_cast<int8_t>((byte >> 2) & 0x03);
                int8_t q2 = static_cast<int8_t>((byte >> 4) & 0x03);
                int8_t q3 = static_cast<int8_t>((byte >> 6) & 0x03);
                size_t baseIdx = superBlock * superBlockSize + subBlock * subBlockSize + i * 4;
                uint8_t qhByte = qh[superBlock * 4 + subBlock];
                uint8_t qhBit0 = (qhByte >> (subBlock * 2)) & 1;
                uint8_t qhBit1 = (qhByte >> (subBlock * 2 + 1)) & 1;
                if (baseIdx < numElements) output[baseIdx] = scale * (q0 | (qhBit0 << 2));
                if (baseIdx + 1 < numElements) output[baseIdx + 1] = scale * (q1 | (qhBit0 << 2));
                if (baseIdx + 2 < numElements) output[baseIdx + 2] = scale * (q2 | (qhBit1 << 2));
                if (baseIdx + 3 < numElements) output[baseIdx + 3] = scale * (q3 | (qhBit1 << 2));
            }
        }
    }
}

void ModelLoader::dequantizeQ3_K(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t superBlockSize = 256;
    const size_t subBlockSize = 32;
    const size_t numSuperBlocks = (numElements + superBlockSize - 1) / superBlockSize;
    output.resize(numElements);

    for (size_t superBlock = 0; superBlock < numSuperBlocks; ++superBlock) {
        const uint8_t* superBlockPtr = data + superBlock * 112;
        const uint8_t* scales = superBlockPtr + 64;
        const uint8_t* qh = scales + 8;
        const uint8_t* qData = qh + 4;

        for (size_t subBlock = 0; subBlock < 4; ++subBlock) {
            const uint8_t* subBlockPtr = qData + superBlock * 64 + subBlock * 8;
            float scale = static_cast<float>(static_cast<int8_t>(scales[superBlock * 4 + subBlock]));

            for (size_t i = 0; i < subBlockSize / 4; ++i) {
                uint8_t byte = subBlockPtr[i];
                int8_t q0 = static_cast<int8_t>(byte & 0x03);
                int8_t q1 = static_cast<int8_t>((byte >> 2) & 0x03);
                int8_t q2 = static_cast<int8_t>((byte >> 4) & 0x03);
                int8_t q3 = static_cast<int8_t>((byte >> 6) & 0x03);
                size_t baseIdx = superBlock * superBlockSize + subBlock * subBlockSize + i * 4;
                uint8_t qhBit0 = (qh[superBlock * 4 + subBlock] >> (i % 8)) & 1;
                uint8_t qhBit1 = (qh[superBlock * 4 + subBlock] >> (i % 8 + 4)) & 1;
                if (baseIdx < numElements) output[baseIdx] = scale * (q0 | (qhBit0 << 2));
                if (baseIdx + 1 < numElements) output[baseIdx + 1] = scale * (q1 | (qhBit0 << 2));
                if (baseIdx + 2 < numElements) output[baseIdx + 2] = scale * (q2 | (qhBit1 << 2));
                if (baseIdx + 3 < numElements) output[baseIdx + 3] = scale * (q3 | (qhBit1 << 2));
            }
        }
    }
}

void ModelLoader::dequantizeQ8_K(const uint8_t* data, size_t numElements, std::vector<float>& output) {
    const size_t superBlockSize = 256;
    const size_t subBlockSize = 32;
    const size_t numSuperBlocks = (numElements + superBlockSize - 1) / superBlockSize;
    output.resize(numElements);

    for (size_t superBlock = 0; superBlock < numSuperBlocks; ++superBlock) {
        const uint8_t* superBlockPtr = data + superBlock * 268;
        const uint8_t* scales = superBlockPtr + 256;
        float min;
        std::memcpy(&min, scales + 8, sizeof(float));

        for (size_t subBlock = 0; subBlock < 8; ++subBlock) {
            float scale = static_cast<float>(static_cast<int8_t>(scales[superBlock * 8 + subBlock]));
            const int8_t* subBlockPtr = reinterpret_cast<const int8_t*>(superBlockPtr) + subBlock * subBlockSize;

            for (size_t i = 0; i < subBlockSize; ++i) {
                size_t idx = superBlock * superBlockSize + subBlock * subBlockSize + i;
                if (idx < numElements) output[idx] = scale * (subBlockPtr[i] - min);
            }
        }
    }
}

bool ModelLoader::dequantizeTensor(const uint8_t* data, size_t numElements, GGUFTensorType type, std::vector<float>& output) {
    switch (type) {
        case GGUFTensorType::F32:
            dequantizeF32(data, numElements, output);
            return true;
        case GGUFTensorType::F16:
            dequantizeF16(data, numElements, output);
            return true;
        case GGUFTensorType::Q4_0:
            dequantizeQ4_0(data, numElements, output);
            return true;
        case GGUFTensorType::Q4_1:
            dequantizeQ4_1(data, numElements, output);
            return true;
        case GGUFTensorType::Q5_0:
            dequantizeQ5_0(data, numElements, output);
            return true;
        case GGUFTensorType::Q5_1:
            dequantizeQ5_1(data, numElements, output);
            return true;
        case GGUFTensorType::Q8_0:
            dequantizeQ8_0(data, numElements, output);
            return true;
        case GGUFTensorType::Q8_1:
            dequantizeQ8_1(data, numElements, output);
            return true;
        case GGUFTensorType::Q4_K:
            dequantizeQ4_K(data, numElements, output);
            return true;
        case GGUFTensorType::Q5_K:
            dequantizeQ5_K(data, numElements, output);
            return true;
        case GGUFTensorType::Q6_K:
            dequantizeQ6_K(data, numElements, output);
            return true;
        case GGUFTensorType::Q2_K:
            dequantizeQ2_K(data, numElements, output);
            return true;
        case GGUFTensorType::Q3_K:
            dequantizeQ3_K(data, numElements, output);
            return true;
        case GGUFTensorType::Q8_K:
            dequantizeQ8_K(data, numElements, output);
            return true;
        default:
            return false;
    }
}

}