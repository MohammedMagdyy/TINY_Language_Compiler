#ifndef TREENODE_H
#define TREENODE_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

struct TreeNode {
    string name;  // Grammar rule or token
    string value; // Optional value for leaf nodes
    vector<shared_ptr<TreeNode>> children;
    vector<shared_ptr<TreeNode>> siblings;

    explicit TreeNode(string name) : name(move(name)) {}
    TreeNode(string name, string value)
        : name(move(name)), value(move(value)) {}
};

#endif // TREENODE_H
