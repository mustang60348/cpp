#include <iostream>
#include <vector>

struct TreeNode {
    int data;
    std::vector<TreeNode*> children;
};

TreeNode* createNode(int data) {
    TreeNode* newNode = new TreeNode;
    newNode->data = data;
    return newNode;
}

void addChild(TreeNode* parent, TreeNode* child) {
    parent->children.push_back(child);
}

void printTree(TreeNode* root) {
    if (root == nullptr) {
        return;
    }

    std::cout << root->data << " ";

    for (TreeNode* child : root->children) {
        printTree(child);
    }
}

int main() {
    // Create the tree
    TreeNode* root = createNode(1);
    TreeNode* child1 = createNode(2);
    TreeNode* child2 = createNode(3);
    TreeNode* grandchild1 = createNode(4);
    TreeNode* grandchild2 = createNode(5);

    addChild(root, child1);
    addChild(root, child2);
    addChild(child1, grandchild1);
    addChild(child1, grandchild2);

    // Print the tree
    std::cout << "Tree: ";
    printTree(root);
    std::cout << std::endl;

    // Clean up memory
    delete root;
    delete child1;
    delete child2;
    delete grandchild1;
    delete grandchild2;

    return 0;
}