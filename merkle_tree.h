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

    MerkleTree(const vector<string>& binaryMessage) {
        root = buildTree(binaryMessage, 0, binaryMessage.size() - 1);
    }

    pair<string, vector<string>> generateVerificationParameters(const vector<string>& binaryMessage, int index) {
        vector<string> pathHashes;
        Node* currentNode = root;
        int start = 0, end = binaryMessage.size() - 1, mid;

        //descend tree to reach target index, each step taking the sibling node hash
        while (start < end) {
            mid = (start + end) / 2;
            if (index <= mid) {
                pathHashes.push_back(currentNode->right->hash);
                currentNode = currentNode->left;
                end = mid;
            } else {
                pathHashes.push_back(currentNode->left->hash);
                currentNode = currentNode->right;
                start = mid + 1;
            }
        }

        reverse(pathHashes.begin(), pathHashes.end());
        return {binaryMessage[index], pathHashes};
    }

private:
    Node* buildTree(const vector<string>& binaryMessage, int start, int end) {
        if (start > end) return nullptr; // handle pointer error
        if (start == end) { //base case for one bit, just hash the bit
            return new Node(tree_hash(binaryMessage[start]));
        }
        
        // recursive case for children
        int mid = (start + end) / 2;
        Node* leftChild = buildTree(binaryMessage, start, mid);
        Node* rightChild = buildTree(binaryMessage, mid + 1, end);
        string combinedHash = tree_hash(leftChild->hash + rightChild->hash);
        Node* node = new Node(combinedHash);
        node->left = leftChild;
        node->right = rightChild;
        return node;
    }
};

bool verify(const string& rootHash, const string& bit, int bitPos, int totalBits, const vector<string>& pathHashes) {
    string currentHash = tree_hash(bit);
    int index = bitPos;

    //iterate from bottom of tree calculating the correct hash at each node
    for (size_t i = 0; i < pathHashes.size(); ++i) {
        currentHash = index % 2 == 0 ? tree_hash(currentHash + pathHashes[i]) : tree_hash(pathHashes[i] + currentHash);
        index /= 2;
    }

    return currentHash == rootHash;
}