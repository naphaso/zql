#include "BTree.h"

using namespace std;

BTreeForest *BTreeForest::_instance = new BTreeForest();
BTreeTraverseHolder *BTreeTraverseHolder::_instance = new BTreeTraverseHolder();


BTreeNode *BTreeForest::getTree(const std::string &database, const std::string &table) {
    string id = database + "." + table;

    map<string, BTreeNode *>::iterator it = _forest.find(id);

    if(it != _forest.end()) {
        return it->second;
    }

    return NULL;
}

BTreeNode::BTreeNode() {
    _left = NULL;
    _right = NULL;
    _ciphertext = NULL;
}

void BTreeForest::createTree(const std::string &database, const std::string &table, Ciphertext *ciphertext) {
    string id = database + "." + table;

    BTreeNode *tree = new BTreeNode();
    tree->ciphertext() = ciphertext;

    _forest[id] = tree;
}


BTreeTraverse::BTreeTraverse(BTreeNode *root, Ciphertext *ciphertext) {
    _current = root;
    _ciphertext = ciphertext;
    _path = 0;
    _pathLength = 0;
    _requestAdd = NULL;
}


bool BTreeTraverse::goToLeft() {
    _path = (_path << 1);
    _pathLength++;

    if(_current->left() != NULL) {
        _current = _current->left();

        return true;
    }

    BTreeNode *newNode = new BTreeNode();
    newNode->ciphertext() = _ciphertext;
    _current->left() = newNode;
    _current = newNode;

    _path = ((_path << 1) | 1) << (63 - _pathLength);

    return false;
}

bool BTreeTraverse::goToRight() {
    _path = (_path << 1) | 1;
    _pathLength++;

    if(_current->right() != NULL) {
        _current = _current->right();
        return true;
    }

    BTreeNode *newNode = new BTreeNode();
    newNode->ciphertext() = _ciphertext;
    _current->right() = newNode;
    _current = newNode;

    _path = ((_path << 1) | 1) << (63 - _pathLength);

    return false;
}

BTreeTraverse *BTreeTraverseHolder::getTraverse(unsigned int requestId) {
    return _traverses[requestId];
}

BTreeTraverse *BTreeTraverseHolder::createTraverse(unsigned int requestId, BTreeNode *root, Ciphertext *ciphertext) {
    BTreeTraverse *traverse = new BTreeTraverse(root, ciphertext);
    _traverses[requestId] = traverse;
    return traverse;
}

Ciphertext *BTreeTraverse::value() {
    return _current->ciphertext();
}

void BTreeTraverseHolder::removeTraverse(unsigned int requestId) {
    map<unsigned int, BTreeTraverse *>::iterator it = _traverses.find(requestId);
    if(it != _traverses.end()) {
        _traverses.erase(it);
        delete it->second;
    }
}

BTreeTraverse::~BTreeTraverse() {
    if(_requestAdd != NULL) {
        delete _requestAdd;
    }
}
