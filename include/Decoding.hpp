#pragma once
#include <string>
#include "openssl/evp.h"
#include "openssl/types.h"
#include <cstddef>
#include <cstdio>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <vector>


inline std::string sha256(const std::string& data) {
    // hash
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;

    // the hashing itself.
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, data.data(), data.size());
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    std::stringstream stream;

    for (size_t i = 0; i < hash_len; i++) {
        stream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return stream.str();
}

inline std::string base_64_encode(const std::string& data) {
    if (data.empty()) return "";

    // Worst-case output size for base64: 4 * ceil(n/3) + null terminator
    size_t max_len = 4 * ((data.size() + 2) / 3) + 1;
    std::vector<unsigned char> out(max_len);

    int written = EVP_EncodeBlock(
        out.data(),
        reinterpret_cast<const unsigned char*>(data.data()),
        static_cast<int>(data.size())
    );

    return std::string(reinterpret_cast<char*>(out.data()), written);
}