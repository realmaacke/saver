#include "auth.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

#include "utils.hpp"
#include "sender.hpp"

#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/decoder.h>

namespace fs = std::filesystem;

EVP_PKEY* loadPrivateKey(const std::string& path) {
    FILE* file = fopen(path.c_str(), "r");
    if (!file) {
        std::cerr << "Failed to open key\n";
        return nullptr;
    }

    EVP_PKEY* key = PEM_read_PrivateKey(file, nullptr, nullptr, nullptr);

    fclose(file);

    if (!key) {
        std::cerr << "Failed to parse PEM key\n";
    }

    return key;
}

std::vector<unsigned char> signMessage(EVP_PKEY* key, const std::string& message) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();

    if (!ctx) return {};

    if (EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, key) != 1) {
        EVP_MD_CTX_free(ctx);
        return {};
    }

    size_t sigLen;
    EVP_DigestSign(ctx, nullptr, &sigLen,
                   (const unsigned char*)message.data(), message.size());

    std::vector<unsigned char> signature(sigLen);

    if (EVP_DigestSign(ctx, signature.data(), &sigLen,
                       (const unsigned char*)message.data(), message.size()) != 1) {
        EVP_MD_CTX_free(ctx);
        return {};
    }

    signature.resize(sigLen);
    EVP_MD_CTX_free(ctx);

    return signature;
}

std::string base64Encode(const std::vector<unsigned char>& data) {
    BIO* bio;
    BIO* b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, data.data(), data.size());
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string result(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);
    return result;
}

std::string iterateConfig(fs::path config_file, std::string config_property) {
    std::fstream read_file;
    read_file.open(config_file);
    
    if (read_file.fail()) {
        std::cerr << "Read file failed" << std::endl;
        return "unknown";
    }
    
    std::vector<std::string> lines;
    std::string line;
    
    while (getline(read_file, line)) {
        if (line.starts_with(config_property + "=")) {
            return line.substr(line.find('=') + 1);
        }
    }
    read_file.close();
    
    return "unknown";
}


/**
 * Creates config and its directory if it dosent exists.
 */
void CreateLocalStorage(std::string config_path, std::string ssh_path) {
    std::string config_file = "keys.saver";
    std::string file_content = "ssh_path=" + ssh_path;

    if (!fs::exists(config_path)) {
        fs::create_directories(config_path);
        std::cout << "[Remove] Created Config at: " << config_path << std::endl;
    }

    if (!fs::exists(config_path + config_file)) {
        std::ofstream outfile (config_path + config_file);

        outfile << file_content << std::endl;

        outfile.close();
    } else {

        std::fstream read_file;
        read_file.open(config_path + config_file);
        
        if (read_file.fail()) {
            std::cerr << "Read file failed" << std::endl;
            return;
        }
        
        std::vector<std::string> lines;
        std::string line;
        
        while (getline(read_file, line)) {
            lines.push_back(line);
        }
        
        read_file.close();
        
        std::ofstream write_file (config_path + config_file);
        bool found = false;

        for (const auto& l : lines) {
            if(l.starts_with("ssh_path=")) {
                write_file << file_content << std::endl;
                found = true;
            } else {
                write_file << l << std::endl;
            }
        }

        if (!found) {
            write_file << file_content << std::endl;
        }

        write_file.close();
    }
}


void Auth::store_key(fs::path ssh_path) {

    CreateLocalStorage(getConfigPath(), ssh_path);
}

void Auth::upload_key(Sender& sender) {
    // takes the public version of the key (to be uploaded).
    fs::path ssh_path = iterateConfig(getConfigPath() + "keys.saver", "ssh_path") + ".pub";

    std::cout << ssh_path << std::endl;

    SendRequest request;

    request.files.push_back(ssh_path);
    request.include_manifest = false;
    request.url = "http://localhost:9019/users/username/invoke_key";

    sender.send(request);
}

void Auth::use_key() {
    fs::path ssh_path = iterateConfig(getConfigPath() + "keys.saver", "ssh_path");

    std::cout << ssh_path.string() << std::endl;

    if (ssh_path.empty()) {
        std::cerr << "no ssh path configured, be sure to run | saver auth path/to/key";
        return;
    }

    EVP_PKEY* key = loadPrivateKey(ssh_path.string());
    
    if (!key) {
        std::cout << "key was undefined" << std::endl;
        return;
    }
    
    std::string message = "ssh_message";
    
    auto signature = signMessage(key, message);
    std::string encoded = base64Encode(signature);
    
    std::cout << "Signature: " << encoded << std::endl;

    EVP_PKEY_free(key);
}