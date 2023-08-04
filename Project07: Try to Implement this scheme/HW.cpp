#include <iostream>
#include <vector>
#include <string>
#include <openssl/sha.h>

class HashWire {
    friend class RangeProof;
public:
    HashWire() {
        head = nullptr;
    }

    void add_certificate(const std::string& certificate) {
        if (head == nullptr) {
            head = new Node{ certificate, "" };
        }
        else {
            Node* new_node = new Node{ certificate, ""};
            new_node->hash = _calculate_hash(new_node->certificate + head->hash);
            head = new_node;
        }
    }

    std::string get_root_hash() const {
        if (head == nullptr) {
            return "";
        }
        return head->hash;
    }

private:
    struct Node {
        std::string certificate;
        std::string hash;
    };

    Node* head;

    std::string _calculate_hash(const std::string& data) const {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, data.c_str(), data.length());
        SHA256_Final(hash, &sha256);

        char buffer[SHA256_DIGEST_LENGTH * 2 + 1];
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            sprintf(&buffer[i * 2], "%02x", hash[i]);
        }

        return std::string(buffer);
    }
};

class RangeProof {
public:
    RangeProof(const std::vector<int>& values) : values(values) {
        for (int value : values) {
            hash_wire.add_certificate(std::to_string(value));
        }
    }

    std::vector<std::string> generate_proof(int start_index, int end_index) {
        std::vector<std::string> proof;

        if (start_index < 0 || end_index >= values.size() || start_index > end_index) {
            return proof;
        }

        HashWire::Node* current_node = hash_wire.head;
        for (size_t i = 0; i < values.size(); ++i) {
            if (i >= static_cast<size_t>(start_index) && i <= static_cast<size_t>(end_index)) {
                proof.push_back(current_node->certificate);
            }
            if (current_node->hash.empty()) {
                break;
            }

            current_node = new HashWire::Node{ current_node->hash, "" };
            current_node->hash = hash_wire._calculate_hash(current_node->certificate + current_node->hash);
        }

        return proof;
    }

private:
    std::vector<int> values;
    HashWire hash_wire;
};

int main() {
    std::vector<int> values = { 1, 2, 3, 4 };
    RangeProof range_proof(values);
    std::vector<std::string> proof = range_proof.generate_proof(1, 2);
    std::cout << "Final result:" << std::endl;
    for (const auto& certificate : proof) {
        std::cout << certificate << std::endl;
    }

    return 0;
}
