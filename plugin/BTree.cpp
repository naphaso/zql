#include "BTree.h"



BTreeNode::BTreeNode() {
	// initialization
}

BTreeNode::~BTreeNode() {
	// recoursive destruction
}

void BTreeNode::GetValues(unsigned int &count, BTreeValue *values) {
	values = this->values;
	count = this->count;
}

void BTreeNode::InsertValue(const unsigned int position, BTreeValue *value) {
	
}
