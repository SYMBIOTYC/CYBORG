#pragma once

#include <cstdint>
#include <string>

namespace cyborg {

constexpr uint32_t GGUF_MAGIC = 0x46554747;

enum class GGUFTensorType : uint32_t {
    F32 = 0,
    F16 = 1,
    Q4_0 = 2,
    Q4_1 = 3,
    Q5_0 = 6,
    Q5_1 = 7,
    Q8_0 = 8,
    Q8_1 = 9,
    Q2_K = 10,
    Q3_K = 11,
    Q4_K = 12,
    Q5_K = 13,
    Q6_K = 14,
    Q8_K = 15,
    IQ2_XXS = 16,
    IQ2_XS = 17,
    IQ3_XXS = 18,
    IQ3_S = 19,
    IQ4_NL = 20,
    IQ4_XS = 21,
    IQ2_S = 22,
    IQ3_XS = 23,
    IQ1_S = 24,
    IQ4_NL_2 = 25,
    IQ1_M = 26,
    IQ4_XS_2 = 27,
};

enum class GGUFValueType : uint32_t {
    GGUF_TYPE_UINT8 = 0,
    GGUF_TYPE_INT8 = 1,
    GGUF_TYPE_UINT16 = 2,
    GGUF_TYPE_INT16 = 3,
    GGUF_TYPE_UINT32 = 4,
    GGUF_TYPE_INT32 = 5,
    GGUF_TYPE_FLOAT32 = 6,
    GGUF_TYPE_DOUBLE = 7,
    GGUF_TYPE_UINT64 = 8,
    GGUF_TYPE_INT64 = 9,
    GGUF_TYPE_BOOL = 10,
    GGUF_TYPE_STRING = 11,
    GGUF_TYPE_ARRAY = 12,
};

constexpr const char* GGUF_METADATA_KEY_ARCHITECTURE = "general.architecture";
constexpr const char* GGUF_METADATA_KEY_NAME = "general.name";
constexpr const char* GGUF_METADATA_KEY_VERSION = "general.version";
constexpr const char* GGUF_METADATA_KEY_DESCRIPTION = "general.description";
constexpr const char* GGUF_METADATA_KEY_LICENSE = "general.license";
constexpr const char* GGUF_METADATA_KEY_URL = "general.url";
constexpr const char* GGUF_METADATA_KEY_PARAMETER_COUNT = "general.parameter_count";
constexpr const char* GGUF_METADATA_KEY_CONTEXT_LENGTH = "general.context_length";
constexpr const char* GGUF_METADATA_KEY_EMBEDDING_LENGTH = "general.embedding_length";
constexpr const char* GGUF_METADATA_KEY_BLOCK_COUNT = "general.block_count";
constexpr const char* GGUF_METADATA_KEY_HEAD_COUNT = "general.head_count";
constexpr const char* GGUF_METADATA_KEY_HEAD_COUNT_KV = "general.head_count_kv";
constexpr const char* GGUF_METADATA_KEY_LAYER_NORM_RMS_EPS = "general.layer_norm_rms_eps";
constexpr const char* GGUF_METADATA_KEY_FREQUENCY_BASE = "general.frequency_base";
constexpr const char* GGUF_METADATA_KEY_FREQUENCY_SCALE = "general.frequency_scale";
constexpr const char* GGUF_METADATA_KEY_ATTENTION_LOGIT_SOFTCAPPING = "general.attention.logit_softcapping";
constexpr const char* GGUF_METADATA_KEY_ATTENTION_ZETA = "general.attention.zeta";
constexpr const char* GGUF_METADATA_KEY_ATTENTION_BETA = "general.attention.beta";
constexpr const char* GGUF_METADATA_KEY_TOKENIZER_TOKENS = "tokenizer.ggml.tokens";
constexpr const char* GGUF_METADATA_KEY_TOKENIZER_TOKEN_TYPE = "tokenizer.ggml.token_type";
constexpr const char* GGUF_METADATA_KEY_TOKENIZER_BOS_TOKEN_ID = "tokenizer.ggml.bos_token_id";
constexpr const char* GGUF_METADATA_KEY_TOKENIZER_EOS_TOKEN_ID = "tokenizer.ggml.eos_token_id";
constexpr const char* GGUF_METADATA_KEY_TOKENIZER_UNKNOWN_TOKEN_ID = "tokenizer.ggml.unknown_token_id";
constexpr const char* GGUF_METADATA_KEY_TOKENIZER_PADDING_TOKEN_ID = "tokenizer.ggml.padding_token_id";
constexpr const char* GGUF_METADATA_KEY_TOKENIZER_ADD_BOS_TOKEN = "tokenizer.ggml.add_bos_token";
constexpr const char* GGUF_METADATA_KEY_TOKENIZER_ADD_EOS_TOKEN = "tokenizer.ggml.add_eos_token";
constexpr const char* GGUF_METADATA_KEY_TOKENIZER_CHAT_TEMPLATE = "tokenizer.ggml.chat_template";

inline const char* tensorTypeToString(GGUFTensorType type) {
    switch (type) {
        case GGUFTensorType::F32: return "F32";
        case GGUFTensorType::F16: return "F16";
        case GGUFTensorType::Q4_0: return "Q4_0";
        case GGUFTensorType::Q4_1: return "Q4_1";
        case GGUFTensorType::Q5_0: return "Q5_0";
        case GGUFTensorType::Q5_1: return "Q5_1";
        case GGUFTensorType::Q8_0: return "Q8_0";
        case GGUFTensorType::Q8_1: return "Q8_1";
        case GGUFTensorType::Q2_K: return "Q2_K";
        case GGUFTensorType::Q3_K: return "Q3_K";
        case GGUFTensorType::Q4_K: return "Q4_K";
        case GGUFTensorType::Q5_K: return "Q5_K";
        case GGUFTensorType::Q6_K: return "Q6_K";
        case GGUFTensorType::Q8_K: return "Q8_K";
        case GGUFTensorType::IQ2_XXS: return "IQ2_XXS";
        case GGUFTensorType::IQ2_XS: return "IQ2_XS";
        case GGUFTensorType::IQ3_XXS: return "IQ3_XXS";
        case GGUFTensorType::IQ3_S: return "IQ3_S";
        case GGUFTensorType::IQ4_NL: return "IQ4_NL";
        case GGUFTensorType::IQ4_XS: return "IQ4_XS";
        case GGUFTensorType::IQ2_S: return "IQ2_S";
        case GGUFTensorType::IQ3_XS: return "IQ3_XS";
        case GGUFTensorType::IQ1_S: return "IQ1_S";
        case GGUFTensorType::IQ4_NL_2: return "IQ4_NL_2";
        case GGUFTensorType::IQ1_M: return "IQ1_M";
        case GGUFTensorType::IQ4_XS_2: return "IQ4_XS_2";
        default: return "UNKNOWN";
    }
}

inline size_t tensorTypeElementSize(GGUFTensorType type) {
    switch (type) {
        case GGUFTensorType::F32: return 4;
        case GGUFTensorType::F16: return 2;
        case GGUFTensorType::Q4_0: return 4;
        case GGUFTensorType::Q4_1: return 4;
        case GGUFTensorType::Q5_0: return 4;
        case GGUFTensorType::Q5_1: return 4;
        case GGUFTensorType::Q8_0: return 4;
        case GGUFTensorType::Q8_1: return 4;
        case GGUFTensorType::Q2_K: return 4;
        case GGUFTensorType::Q3_K: return 4;
        case GGUFTensorType::Q4_K: return 4;
        case GGUFTensorType::Q5_K: return 4;
        case GGUFTensorType::Q6_K: return 4;
        case GGUFTensorType::Q8_K: return 4;
        case GGUFTensorType::IQ2_XXS: return 4;
        case GGUFTensorType::IQ2_XS: return 4;
        case GGUFTensorType::IQ3_XXS: return 4;
        case GGUFTensorType::IQ3_S: return 4;
        case GGUFTensorType::IQ4_NL: return 4;
        case GGUFTensorType::IQ4_XS: return 4;
        case GGUFTensorType::IQ2_S: return 4;
        case GGUFTensorType::IQ3_XS: return 4;
        case GGUFTensorType::IQ1_S: return 4;
        case GGUFTensorType::IQ4_NL_2: return 4;
        case GGUFTensorType::IQ1_M: return 4;
        case GGUFTensorType::IQ4_XS_2: return 4;
        default: return 0;
    }
}

}