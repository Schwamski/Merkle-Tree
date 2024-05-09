#include "../include/hash.h"


//! ALL HASHING IS 64-BIT

// bitwise shifting helper for 64-bit value
unsigned long long rotate_left(unsigned long long value, int shift) {
    return (value << shift) | (value >> (64 - shift));
}

std::string Hash::hash(const std::string& input) {
    return input;
}

std::string DummyHash::hash(const std::string& input) {
    return input;
}

std::string PrimeHash::hash(const std::string& input) {
    unsigned long long hash = 0;
    for (char c : input) {
        hash = rotate_left(hash, 5) + (c * 131);
    }
    return std::to_string(hash);
}

std::string Djb2Hash::hash(const std::string& input) {
    unsigned long long hash = 5381;
    for (char c : input) {
        hash = rotate_left(hash, 5) ^ (c * 0x5DEECE66DLL);
    }
    return std::to_string(hash);
}