#include <string>
#include <sstream>
#include <iomanip>

//! ALL HASHING IS 64-BIT

// bitwise shifting helper for 64-bit value
unsigned long long rotate_left(unsigned long long value, int shift) {
    return (value << shift) | (value >> (64 - shift));
}

// dummy hash for testing weak hashing
std::string dummy_hash(const std::string& input) {
    return input;
}

// prime hash function
std::string prime_hash(const std::string& input) {
    unsigned long long hash = 0;
    for (char c : input) {
        hash = rotate_left(hash, 5) + (c * 131);
    }
    return std::to_string(hash);
}

// djb2 hash function
std::string djb2_hash(const std::string& input) {
    unsigned long long hash = 5381;
    for (char c : input) {
        hash = rotate_left(hash, 5) ^ (c * 0x5DEECE66DLL);
    }
    return std::to_string(hash);
}

// tree hash function calls designated hashing method
std::string tree_hash(const std::string& input) {
    return djb2_hash(input);
}