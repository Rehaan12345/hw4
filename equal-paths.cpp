#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int pathHeight(Node* root);

// returns the leaf-to-root path length of this subtree,
// or -1 if its leaves are at different depths
int pathHeight(Node* root)
{
    if(root == nullptr) return 0;

    int left = pathHeight(root->left);
    int right = pathHeight(root->right);
    if(left == -1 || right == -1) return -1;

    // only one child: the only paths go through that child
    if(root->left == nullptr) return right + 1;
    if(root->right == nullptr) return left + 1;

    if(left != right) return -1;
    return left + 1;
}

bool equalPaths(Node * root)
{
    // Add your code below
    return pathHeight(root) != -1;
}

