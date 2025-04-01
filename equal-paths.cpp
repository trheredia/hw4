#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;

// You may add any prototypes of helper functions here
bool pathHelper(Node* node, int depth, int& leaf){
  
  if(node == nullptr){ // base case if the node is empty
    return true;
  }

  if(node->left == nullptr && node->right == nullptr){ // if its a leaf node
    if(leaf == -1){ // remember the depth if it's the first leaf
      leaf = depth;
    }
    if(depth == leaf){ // compare the depths  
      return true;
    } 
    else{
      return false;
    }
  }

  bool leftRecursion; 
  leftRecursion = pathHelper(node->left, depth + 1, leaf); // recurse throught left subtree
  bool rightRecursion;
  rightRecursion = pathHelper(node->right, depth + 1, leaf); // recurse through right subtree

  if(leftRecursion && rightRecursion){ // return if both are true
    return true;
  }
  else{
    return false;
  }
}


bool equalPaths(Node * root)
{
    // Add your code below
    int leaf = -1; // no leaf yet
    return pathHelper(root, 0, leaf); // recurse starting at depth 0

}

