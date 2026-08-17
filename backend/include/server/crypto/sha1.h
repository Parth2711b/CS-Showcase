#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace crypto {

inline uint32_t left_rotate(uint32_t value, size_t count){
    return (value << count) | (value >> (32 - count));
}

inline std::vector<uint8_t> sha1(const std::string& input){
    std::vector<uint8_t> data(input.begin(), input.end());
    uint64_t bit_len = data.size() * 8;
    data.push_back(0x80);
    while (data.size() % 64 != 56) data.push_back(0x00);
    for (int i = 7; i >= 0; --i) data.push_back((bit_len >> (i * 8)) & 0xFF);
    uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89, h2 = 0x98BADCFE, h3 = 0x10325476, h4 = 0xC3D2E1F0;
    for (size_t i = 0; i < data.size(); i += 64){
        uint32_t w[80];
        for (int j = 0; j < 16; ++j){
            w[j]=(data[i + j * 4] << 24)|(data[i + j * 4 + 1] << 16)|(data[i + j * 4 + 2] << 8)|(data[i + j * 4 + 3]);
        }
        for (int j = 16; j < 80; ++j) w[j] = left_rotate(w[j - 3] ^ w[j - 8] ^ w[j - 14] ^ w[j - 16], 1);
        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
        for (int j = 0; j < 80; ++j){
            uint32_t f, k;
            if (j < 20) { f = (b & c) | ((~b) & d); k = 0x5A827999; }
            else if (j < 40) { f = b ^ c ^ d; k = 0x6ED9EBA1; }
            else if (j < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
            else { f = b ^ c ^ d; k = 0xCA62C1D6; }
            uint32_t temp = left_rotate(a, 5) + f + e + k + w[j];
            e = d; d = c; c = left_rotate(b, 30); b = a; a = temp;
        }
        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    }

    std::vector<uint8_t> hash(20);
    for (int i = 0; i < 4; ++i) hash[i] = (h0 >> (24 - i * 8)) & 0xFF;
    for (int i = 0; i < 4; ++i) hash[i + 4] = (h1 >> (24 - i * 8)) & 0xFF;
    for (int i = 0; i < 4; ++i) hash[i + 8] = (h2 >> (24 - i * 8)) & 0xFF;
    for (int i = 0; i < 4; ++i) hash[i + 12] = (h3 >> (24 - i * 8)) & 0xFF;
    for (int i = 0; i < 4; ++i) hash[i + 16] = (h4 >> (24 - i * 8)) & 0xFF;
    return hash;
}

} // namespace crypto
