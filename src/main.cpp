#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <cassert>
#include "../include/merkle_tree.h"

using namespace std;

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
    return 0;
}
