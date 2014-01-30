#ifndef BTREE_H_
#define BTREE_H_

#include <string>
#include <map>
#include "Request.h"

class Ciphertext {
private:
    unsigned char *_data;
    unsigned int _size;
public:
    Ciphertext(unsigned char *data, unsigned int size):_data(data), _size(size) {}
    unsigned char *data() { return _data; }
    unsigned int size() { return _size; }
};


class BTreeNode {
private:
    // TODO: data for balancing
    Ciphertext *_ciphertext;
    BTreeNode *_left, *_right;
public:
    BTreeNode();

    BTreeNode *&left() { return _left; }
    BTreeNode *&right() { return _right; }
    Ciphertext *&ciphertext() { return _ciphertext; }
};

class BTreeForest {
private:
    static BTreeForest *_instance;
    std::map<std::string, BTreeNode *> _forest;
public:
    static BTreeForest *instance() { return _instance; }
    BTreeNode *getTree(const std::string &database, const std::string &table);
    void createTree(const std::string &database, const std::string &table, Ciphertext *ciphertext);
};


class BTreeTraverse {
private:
    Ciphertext *_ciphertext;
    BTreeNode *_current;
    unsigned long long _path;
    int _pathLength;

    RequestAdd *_requestAdd;

public:
    BTreeTraverse(BTreeNode *root, Ciphertext *ciphertext);
    ~BTreeTraverse();

    bool goToLeft();
    bool goToRight();

    Ciphertext *value();
    unsigned long long path() { return _path; }

    RequestAdd *&requestAdd() { return _requestAdd; }
};

class BTreeTraverseHolder {
private:
    static BTreeTraverseHolder *_instance;
    std::map<unsigned int, BTreeTraverse *> _traverses;
public:
    static BTreeTraverseHolder *instance() { return _instance; }
    BTreeTraverse *getTraverse(unsigned int requestId);
    BTreeTraverse *createTraverse(unsigned int requestId, BTreeNode *root, Ciphertext *ciphertext);
    void removeTraverse(unsigned int requestId);
};

#endif
