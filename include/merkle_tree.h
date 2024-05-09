#ifndef MERKLE_TREE_H
#define MERKLE_TREE_H

#include <iostream>
#include <string>
#include <vector>
#include "hash.h"

using namespace std;

struct Node {
    string hash;
    Node *left;
    Node *right;

    Node() : hash(""), left(nullptr), right(nullptr) {}
    Node(const string& h) : hash(h), left(nullptr), right(nullptr) {}
};

class MerkleTree {
public:
    Node *root;
    Hash hash = Djb2Hash();

    MerkleTree() : root(nullptr) {}
    MerkleTree(const vector<string>& binaryMessage);

    pair<string, vector<string> > generateVerificationParameters(const vector<string>& binaryMessage, int index);

private:
    Node* buildTree(const vector<string>& binaryMessage, int start, int end);
};


class Verifier {
public:
    Verifier(MerkleTree tree);
    bool verify(const string& rootHash, const string& bit, int bitPos, int totalBits, const vector<string>& pathHashes);

private:
    MerkleTree tree;
};

#endif