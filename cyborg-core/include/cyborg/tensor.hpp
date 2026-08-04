#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace cyborg {

class Tensor {
public:
    std::string name;
    std::vector<size_t> shape;

    Tensor() = default;

    Tensor(const std::vector<size_t>& shape, const std::vector<float>& data)
        : shape(shape)
        , m_data(data) {
        validate();
    }

    Tensor(const std::vector<size_t>& shape, float fill_value = 0.0f)
        : shape(shape) {
        m_data.resize(numElements(), fill_value);
    }

    size_t rank() const { return shape.size(); }

    size_t numElements() const {
        size_t n = 1;
        for (auto d : shape) n *= d;
        return n;
    }

    const std::vector<float>& data() const { return m_data; }
    std::vector<float>& data() { return m_data; }

    float& at(const std::vector<size_t>& indices) {
        return m_data[flatIndex(indices)];
    }

    const float& at(const std::vector<size_t>& indices) const {
        return m_data[flatIndex(indices)];
    }

    float& operator()(const std::vector<size_t>& indices) {
        return at(indices);
    }

    const float& operator()(const std::vector<size_t>& indices) const {
        return at(indices);
    }

    Tensor operator+(const Tensor& other) const {
        if (shape != other.shape) {
            throw std::invalid_argument("Tensor shape mismatch for addition");
        }
        Tensor result(shape);
        for (size_t i = 0; i < m_data.size(); ++i) {
            result.m_data[i] = m_data[i] + other.m_data[i];
        }
        return result;
    }

    Tensor operator*(const Tensor& other) const {
        if (shape != other.shape) {
            throw std::invalid_argument("Tensor shape mismatch for multiplication");
        }
        Tensor result(shape);
        for (size_t i = 0; i < m_data.size(); ++i) {
            result.m_data[i] = m_data[i] * other.m_data[i];
        }
        return result;
    }

    Tensor operator*(float scalar) const {
        Tensor result(shape);
        for (size_t i = 0; i < m_data.size(); ++i) {
            result.m_data[i] = m_data[i] * scalar;
        }
        return result;
    }

    Tensor superposition(const Tensor& other) const {
        if (shape != other.shape) {
            throw std::invalid_argument("Tensor shape mismatch for superposition");
        }
        Tensor result(shape);
        for (size_t i = 0; i < m_data.size(); ++i) {
            result.m_data[i] = m_data[i] + other.m_data[i];
        }
        return result;
    }

    Tensor normalize() const {
        float norm = 0.0f;
        for (auto v : m_data) {
            norm += v * v;
        }
        norm = std::sqrt(norm);
        if (norm < 1e-8f) return *this;
        return *this * (1.0f / norm);
    }

    Tensor slice(size_t dim, size_t index) const {
        if (dim >= shape.size()) {
            throw std::out_of_range("Slice dimension out of range");
        }
        if (index >= shape[dim]) {
            throw std::out_of_range("Slice index out of range");
        }
        std::vector<size_t> new_shape = shape;
        new_shape.erase(new_shape.begin() + dim);
        size_t stride = 1;
        for (size_t i = dim + 1; i < shape.size(); ++i) {
            stride *= shape[i];
        }
        std::vector<float> new_data;
        new_data.reserve(numElements() / shape[dim]);
        for (size_t i = 0; i < m_data.size(); ++i) {
            size_t dim_idx = (i / stride) % shape[dim];
            if (dim_idx == index) {
                new_data.push_back(m_data[i]);
            }
        }
        return Tensor(new_shape, new_data);
    }

    Tensor clone() const {
        return Tensor(shape, m_data);
    }

    float dot(const Tensor& other) const {
        if (m_data.size() != other.m_data.size()) {
            throw std::invalid_argument("Tensor size mismatch for dot product");
        }
        float result = 0.0f;
        for (size_t i = 0; i < m_data.size(); ++i) {
            result += m_data[i] * other.m_data[i];
        }
        return result;
    }

    Tensor matmul(const Tensor& other) const {
        if (rank() != 2 || other.rank() != 2) {
            throw std::invalid_argument("Matmul requires 2D tensors");
        }
        if (shape[1] != other.shape[0]) {
            throw std::invalid_argument("Matmul shape mismatch");
        }
        size_t M = shape[0];
        size_t K = shape[1];
        size_t N = other.shape[1];
        Tensor result({M, N}, 0.0f);
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                float sum = 0.0f;
                for (size_t k = 0; k < K; ++k) {
                    sum += (*this)({i, k}) * other({k, j});
                }
                result({i, j}) = sum;
            }
        }
        return result;
    }

    Tensor reshape(const std::vector<size_t>& new_shape) const {
        size_t new_numel = 1;
        for (auto d : new_shape) new_numel *= d;
        if (new_numel != numElements()) {
            throw std::invalid_argument("Reshape total elements mismatch");
        }
        return Tensor(new_shape, m_data);
    }

    Tensor transpose() const {
        if (rank() != 2) {
            throw std::invalid_argument("Transpose requires 2D tensor");
        }
        std::vector<size_t> new_shape = {shape[1], shape[0]};
        Tensor result(new_shape);
        for (size_t i = 0; i < shape[0]; ++i) {
            for (size_t j = 0; j < shape[1]; ++j) {
                result({j, i}) = (*this)({i, j});
            }
        }
        return result;
    }

private:
    std::vector<float> m_data;

    void validate() const {
        if (shape.empty()) return;
        size_t expected = 1;
        for (auto d : shape) expected *= d;
        if (m_data.size() != expected) {
            throw std::invalid_argument("Tensor data size does not match shape");
        }
    }

    size_t flatIndex(const std::vector<size_t>& indices) const {
        if (indices.size() != shape.size()) {
            throw std::out_of_range("Index dimension mismatch");
        }
        size_t idx = 0;
        size_t stride = 1;
        for (size_t i = shape.size(); i-- > 0; ) {
            if (indices[i] >= shape[i]) {
                throw std::out_of_range("Index out of range");
            }
            idx += indices[i] * stride;
            stride *= shape[i];
        }
        return idx;
    }
};

} // namespace cyborg