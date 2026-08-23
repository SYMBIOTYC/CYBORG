#pragma once

#include <cyborg/tensor.hpp>
#include <vector>
#include <cmath>

namespace cyborg {

inline float dot(const Tensor& a, const Tensor& b) {
    return a.dot(b);
}

inline Tensor matmul(const Tensor& a, const Tensor& b) {
    return a.matmul(b);
}

inline Tensor reshape(const Tensor& t, const std::vector<size_t>& new_shape) {
    return t.reshape(new_shape);
}

inline Tensor transpose(const Tensor& t) {
    return t.transpose();
}

inline Tensor normalize(const Tensor& t) {
    return t.normalize();
}

inline Tensor elementwise_multiply(const Tensor& a, const Tensor& b) {
    return a * b;
}

inline Tensor elementwise_add(const Tensor& a, const Tensor& b) {
    return a + b;
}

inline Tensor scale(const Tensor& t, float s) {
    return t * s;
}

inline float tensor_sum(const Tensor& t) {
    float sum = 0.0f;
    for (auto v : t.data()) {
        sum += v;
    }
    return sum;
}

inline float tensor_mean(const Tensor& t) {
    if (t.numElements() == 0) return 0.0f;
    return tensor_sum(t) / static_cast<float>(t.numElements());
}

inline float tensor_max(const Tensor& t) {
    float max_val = t.data().empty() ? 0.0f : t.data()[0];
    for (auto v : t.data()) {
        if (v > max_val) max_val = v;
    }
    return max_val;
}

inline float tensor_min(const Tensor& t) {
    float min_val = t.data().empty() ? 0.0f : t.data()[0];
    for (auto v : t.data()) {
        if (v < min_val) min_val = v;
    }
    return min_val;
}

} // namespace cyborg