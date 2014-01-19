#ifndef BTREE_H_
#define BTREE_H_

class BTreeValue {

};

class BTree {
private:
	BTreeNode *root;
};


class BTreeNode {
private:
	BTreeNode *parent;
	unsigned int positionInParent;
	BTreeValue *values[255];
	unsigned int count;
	BTreeNode *childs[256];
public:
	BTreeNode();
	~BTreeNode();
	void GetValues(unsigned int &count, BTreeValue *values);
	void InsertValue(const unsigned int position, BTreeValue *value);
};

#endif
