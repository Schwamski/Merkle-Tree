#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <cassert>
#include "../include/merkle_tree.h"

using namespace std;

// Function to calculate the next highest power of 2
unsigned nextPowerOf2(unsigned n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

// Convert int to binary vector<string> padded to length
vector<string> intToBinaryVector(int number, int length) {
    vector<string> binaryMessage(length, "0");
    int index = length - 1;
    while (number > 0 && index >= 0) {
        binaryMessage[index--] = to_string(number % 2);
        number /= 2;
    }
    return binaryMessage;
}

// Helper to save message-roothash pairs to csv for later analysis
void saveToCSV(const string& binaryMessage, const string& rootHash, ofstream& outFile) {
    outFile << binaryMessage << "," << rootHash << "\n";
}

// Test a single, untampered, randomly generated commitment (16 bit)
void testPassVerification(mt19937& gen, uniform_int_distribution<>& distrib, uniform_int_distribution<>& bitDistrib, ofstream& outFile) {
    //generate random commitment message
    int num = distrib(gen);
    vector<string> binaryMessage = intToBinaryVector(num, 16);

    // Generate merkle tree & verification params. save resulting message-commitment pair
    MerkleTree tree(binaryMessage);
    Verifier verifier(tree);
    string rootHash = tree.root->hash;
    saveToCSV(std::to_string(num), rootHash, outFile); // Save binary message and root hash to CSV

    // Verify
    int choiceBit = bitDistrib(gen);
    auto [bitValue, verificationParams] = tree.generateVerificationParameters(binaryMessage, choiceBit);
    bool result = verifier.verify(tree.root->hash, bitValue, choiceBit, binaryMessage.size(), verificationParams);
    assert(result);
}

// Test a single, TAMPERED, randomly generated commitment (16 bit)
void testFailVerification(mt19937& gen, uniform_int_distribution<>& distrib, uniform_int_distribution<>& bitDistrib) {
    //generate random commitment message
    int num = distrib(gen);
    vector<string> binaryMessage = intToBinaryVector(num, 16);

    // Generate merkle tree & verification params
    MerkleTree tree(binaryMessage);
    Verifier verifier(tree);
    int choiceBit = bitDistrib(gen);
    auto [bitValue, verificationParams] = tree.generateVerificationParameters(binaryMessage, choiceBit);

    // Flip chosen bit to simulate tampering
    string tamperedBitValue = bitValue == "1" ? "0" : "1";

    // Verify
    bool result = verifier.verify(tree.root->hash, tamperedBitValue, choiceBit, binaryMessage.size(), verificationParams);
    assert(!result);
}

int main() {
    int test_samples = 10000;
    ofstream outFile("merkle_data.csv");
    outFile << "BinaryMessage,RootHash\n";

    // Parameters to generate random numbers
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, (1 << 16) - 1);
    uniform_int_distribution<> bitDistrib(0, 15);

    // Run test_sample test cases for passed and failed verifications
    for (int i = 0; i < test_samples; ++i) {
        testPassVerification(gen, distrib, bitDistrib, outFile);
        testFailVerification(gen, distrib, bitDistrib);
    }

    outFile.close();
    cout << "CSV created with " << test_samples << " entries." << endl;

    // user input binary message
    std::string input;
    std::cout << "Enter a string: ";
    std::getline(std::cin, input);
    std::vector<std::string> binaryMessage;
    for (char c : input) {
        std::bitset<8> binary(c);
        for (int i = 7; i >= 0; --i) {
            binaryMessage.push_back(binary.test(i) ? "1" : "0");
        }
    }

    // print binary
    std::cout << "Binary Message: ";
    for (const std::string& bit : binaryMessage) {
        std::cout << bit;
    }
    std::cout << std::endl;

    // pad binary and create Merkle Tree
    unsigned requiredSize = nextPowerOf2(binaryMessage.size());
    binaryMessage.resize(requiredSize, "0");
    MerkleTree tree(binaryMessage);
    std::cout << "Root Hash: " << tree.root->hash << std::endl;

    // Verify a bit using Merkle tree
    Verifier verifier(tree);
    for (int i = 0; i < 4; ++i) {
        int choiceBit;
        std::cout << "Choose bit to verify: ";
        std::cin >> choiceBit;
        std::cin.ignore();
        auto [bitValue, verificationParams] = tree.generateVerificationParameters(binaryMessage, choiceBit);
        bool result = verifier.verify(tree.root->hash, bitValue, choiceBit, binaryMessage.size(), verificationParams);
        std::cout << "Verification result: " << (result ? "True" : "False") << std::endl;
    }

    //--------TAMPER WITH MESSAGE
    // user input binary message
    std::string newInput;
    std::cout << "Try to change the secret message: ";
    std::getline(std::cin, newInput);
    std::vector<std::string> newMessage;
    for (char c : newInput) {
        std::bitset<8> binary(c);
        for (int i = 7; i >= 0; --i) {
            newMessage.push_back(binary.test(i) ? "1" : "0");
        }
    }

    // print binary
    std::cout << "Binary Message: ";
    for (const std::string& bit : newMessage) {
        std::cout << bit;
    }
    std::cout << std::endl;

    // Verify a bit using Merkle tree
    for (int i = 0; i < 4; ++i) {
        int choiceBit;
        std::cout << "Choose bit to verify: ";
        std::cin >> choiceBit;
        std::cin.ignore();
        auto [bitValue, verificationParams] = tree.generateVerificationParameters(newMessage, choiceBit);
        bool result = verifier.verify(tree.root->hash, bitValue, choiceBit, binaryMessage.size(), verificationParams);
        std::cout << "Verification result: " << (result ? "True" : "False") << std::endl;
    }
    
    return 0;
}
