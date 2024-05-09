#include "../include/merkle_tree.h"

MerkleTree::MerkleTree(const vector<string>& binaryMessage) {
    root = buildTree(binaryMessage, 0, binaryMessage.size() - 1);
}

pair<string, vector<string> > MerkleTree::generateVerificationParameters(const vector<string>& binaryMessage, int index) {
    vector<string> pathHashes;
    Node* currentNode = root;
    int start = 0, end = binaryMessage.size() - 1, mid;

    // Descend tree to reach target index, each step taking the sibling node hash
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
    return std::make_pair(binaryMessage[index], pathHashes);
}

Node* MerkleTree::buildTree(const vector<string>& binaryMessage, int start, int end) {
    if (start > end) return nullptr; // Handle pointer error
    if (start == end) { 
        // Base case for one bit, just hash the bit
        return new Node(hash.hash(binaryMessage[start]));
    }
    
    // Recursive case for children
    int mid = (start + end) / 2;
    Node* leftChild = buildTree(binaryMessage, start, mid);
    Node* rightChild = buildTree(binaryMessage, mid + 1, end);
    string combinedHash = hash.hash(leftChild->hash + rightChild->hash);
    Node* node = new Node(combinedHash);
    node->left = leftChild;
    node->right = rightChild;
    return node;
}

Verifier::Verifier(MerkleTree tree) {
    Verifier::tree = tree;
}

bool Verifier::verify(const string& rootHash, const string& bit, int bitPos, int totalBits, const vector<string>& pathHashes) {
    string currentHash = tree.hash.hash(bit);
    int index = bitPos;

    // Iterate from bottom of tree calculating the correct hash at each node
    for (size_t i = 0; i < pathHashes.size(); ++i) {
        currentHash = index % 2 == 0 ? tree.hash.hash(currentHash + pathHashes[i]) : tree.hash.hash(pathHashes[i] + currentHash);
        index /= 2;
    }

    return currentHash == rootHash;
}