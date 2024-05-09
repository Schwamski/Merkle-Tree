#ifndef HASH_H
#define HASH_H

#include <string>
#include <sstream>
#include <iomanip>


// Parent hash class
class Hash {
    public:
    virtual std::string hash(const std::string& input);
};

// Dummy hash for testing weak hashing
class DummyHash : public Hash {
    public:
    std::string hash(const std::string& input);
};

// Prime hash function
class PrimeHash : public Hash {
    public:
    std::string hash(const std::string& input);
};

// djb2 hash function
class Djb2Hash : public Hash {
    public:
    std::string hash(const std::string& input);
};

#endif