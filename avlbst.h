#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void leftRotation(AVLNode<Key, Value>* node);
    void rightRotation(AVLNode<Key, Value>* node);
    void rebalanceHelper(AVLNode<Key, Value>* node);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    AVLNode<Key, Value>* currentNode = static_cast<AVLNode<Key, Value>*>(this->root_); // sett current node at the root
    AVLNode<Key, Value>* parentNode = nullptr; // set the parent node to null for now 

    while(currentNode != nullptr){ // traverse through the tree 
      parentNode = currentNode; // remember the parent node 
      if(new_item.first < currentNode->getKey()){ // if the key is smaller, go left
        currentNode = currentNode->getLeft();
      }
      else if(new_item.first > currentNode->getKey()){ // if the key is larger, go right 
        currentNode = currentNode->getRight();
      }
      else{ // otherwise the key already exists so just update the value 
        currentNode->setValue(new_item.second); 
        return;
      }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parentNode); // create the new node to insert into
    if(parentNode == nullptr){ // new node becomes the root if the tree was empty
      this->root_ = newNode;
      return;
    }
    else if(new_item.first < parentNode->getKey()){ // if the key is less than the parent's key, then the node becomes a left child 
      parentNode->setLeft(newNode);
    }
    else{ // if the key is greater than the parent's key, then the node becomes a right child 
      parentNode->setRight(newNode);
    }

    AVLNode<Key, Value>* node = newNode; // set node to start rebalancing from 
    while(parentNode != nullptr){ 
      if(node == parentNode->getLeft()){ // if the new node is a left child, then subtract 1 from the balance 
        parentNode->updateBalance(-1);
      }
      else{ // if the new node is a right child, then add 1 to the balance
        parentNode->updateBalance(1);
      }

      int8_t balance = parentNode->getBalance(); // get the current balance 

      if(balance == 0){ // break if it's balanced 
        break;
      }
      else if(balance == -2 || balance == 2){ // if it's unbalanced, rebalance with helper and then break
        rebalanceHelper(parentNode);
        break;
      }
   
      node = parentNode; // move the node to the parent 
      parentNode = parentNode->getParent(); // reset the parent node 
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key)); // find the node to remove 
    if(node == nullptr){ // if the node is empty do nothing 
      return;
    }

    if(node->getLeft() != nullptr && node->getRight() != nullptr){ // if the node has two children
      AVLNode<Key, Value>* predecessorNode = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
      this->nodeSwap(node, predecessorNode); // swap it with its predecessor 
    }

    AVLNode<Key, Value>* childNode = nullptr; // start search for child node by defaulting it to null 
    if(node->getLeft() != nullptr){ // look for left child 
      childNode = node->getLeft();
    }
    else if(node->getRight() != nullptr){ // look for right child 
      childNode = node->getRight();
    }

    AVLNode<Key, Value>* parentNode = node->getParent(); // reset the parent and child nodes
    bool removed = false;
    if(childNode != nullptr){
      childNode->setParent(parentNode);
    }

    if(parentNode == nullptr){ // if the removed node was the root then the child becomes the root 
      this->root_ = childNode;
    }
    else{
      if(parentNode->getLeft() == node){ // if the removed node was a left child then reset the child 
        parentNode->setLeft(childNode);
        removed = true; // note that it was removed from the left!!!!
      }
      else{
        parentNode->setRight(childNode); // if the removed node was a right child the reset the child 
      }
    }

    delete node; // delete the node 

    AVLNode<Key, Value>* currentNode = parentNode; // set the current node for rebalancing 
    while(currentNode != nullptr){ // traverse through the tree 
      int8_t balance = currentNode->getBalance();

      if(removed){ // if the node was removed from the left then we increase the balance 
        currentNode->updateBalance(1);
      }
      else{ // otherwise it was removed from the right, so we decrease the balance 
        currentNode->updateBalance(-1);
      }

      balance = currentNode->getBalance(); // get the updated balance 

      if(balance == -1 || balance == 1){ // don't need to do anything 
        break;
      }
      else if(balance == 2 || balance == -2){ // if it's unbalanced 
        AVLNode<Key, Value>* nextNode = currentNode->getParent();
        rebalanceHelper(currentNode); // rotations 

        currentNode = nextNode; // move the current node to the parent 

        if(currentNode == nullptr){ // if we have reached the root do nothing 
          break;
        }

        if(currentNode->getLeft() == node){ // if the node was removed from the left 
          removed = true;
        }
        else{
          removed = false;
        }
      }
      else{ // if the balance is 0
        AVLNode<Key, Value>* nextNode = currentNode->getParent();

        if(nextNode == nullptr){ // if we have reached the root do nothing 
          break;
        }
        if(nextNode->getLeft() == currentNode){ // if the node was removed from the left 
          removed = true;
        }
        else{
          removed = false;
        }
        currentNode = nextNode; // move to the next node 
      }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

// adding my rotation helper functions
template<class Key, class Value>
void AVLTree<Key, Value>::leftRotation(AVLNode<Key, Value>* node)
{
  AVLNode<Key, Value>* rightChild = node->getRight(); // get the right child of the node 
  if(!rightChild){ // if it doesn't exist then do nothing 
    return;
  }

  AVLNode<Key, Value>* parentNode = node->getParent();
  node->setRight(rightChild->getLeft()); // reset the right pointer 

  if(rightChild->getLeft()){ // if there is a subtree then also reset its parent 
    rightChild->getLeft()->setParent(node);
  }

  rightChild->setLeft(node); // resetting node pointers 
  node->setParent(rightChild);
  rightChild->setParent(parentNode);

  if(parentNode == nullptr){ // if the node was the root then reset the root 
    this->root_ = rightChild;
  }
  else if(parentNode->getLeft() == node){ // node was left child 
    parentNode->setLeft(rightChild);
  }
  else{ // node was right child 
    parentNode->setRight(rightChild); 
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rightRotation(AVLNode<Key, Value>* node)
{
  AVLNode<Key, Value>* leftChild = node->getLeft(); // get the left child of the node 
  if(!leftChild){ // do nothing if it doesn't exist 
    return;
  }

  AVLNode<Key, Value>* parentNode = node->getParent();
  node->setLeft(leftChild->getRight());// reset the left pointer 

  if(leftChild->getRight()){ // if there is a subtree then also reset its parent 
    leftChild->getRight()->setParent(node);
  }

  leftChild->setRight(node); // resetting node pointers 
  node->setParent(leftChild);
  leftChild->setParent(parentNode);

  if(parentNode == nullptr){ // if the node was the root then reset the root 
    this->root_ = leftChild;
  }
  else if(parentNode->getLeft() == node){ // if the node was a left child 
    parentNode->setLeft(leftChild);
  }
  else{ // if the node was a right child 
    parentNode->setRight(leftChild);
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceHelper(AVLNode<Key, Value>* node)
{
  int8_t balance = node->getBalance(); // get the current balance 

  if(balance == -2){ // if the left subtree is taller 
    AVLNode<Key, Value>* left = node->getLeft(); // get the left node 
    if(left == nullptr){
      return;
    }
    if(left->getBalance() <= 0){ // left left zig zig case 
      rightRotation(node); // perform one rotation
      node->setBalance(0); // reset the balances after rotating 
      left->setBalance(0);
    }
    else{ // left right zig zag case 
      AVLNode<Key, Value>* grandchild = left->getRight(); // get the grandchild node 
      leftRotation(left); // perform two rotations 
      rightRotation(node);
      if(grandchild != nullptr){
        int8_t grandchildBalance = grandchild->getBalance(); // get the balance 
        if(grandchildBalance == -1){ // reset balances because its taller on the right 
          node->setBalance(1);
          left->setBalance(0);
        }
        else if(grandchildBalance == 1){ // reset balances because taller on the left 
          node->setBalance(0);
          left->setBalance(-1);
        }
        else{ // balance is fine 
          node->setBalance(0);
          left->setBalance(0);
        }
        grandchild->setBalance(0); // reset grandchild balance 
      }
    }
  }
  else if(balance == 2){ // if the right subtree is taller  
    AVLNode<Key, Value>* right = node->getRight();
    if(right == nullptr){
      return;
    }

    if(right->getBalance() >= 0){ // right right zig zig case 
      leftRotation(node); // perform one rotation 
      node->setBalance(0); // reset the balances 
      right->setBalance(0);
    }
    else{ // right left zig zag case 
      AVLNode<Key, Value>* grandchild = right->getLeft(); // get the grandchild 
      rightRotation(right); // perform two rotations 
      leftRotation(node);
      if(grandchild != nullptr){
        int8_t grandchildBalance = grandchild->getBalance(); // get the balance 
        if(grandchildBalance == 1){ // reset balances according to left being taller 
          node->setBalance(-1);
          right->setBalance(0);
        }
        else if(grandchildBalance == -1){ // reset balances according to right being taller 
          node->setBalance(0);
          right->setBalance(1);
        }
        else{ // balance is fine 
          node->setBalance(0);
          right->setBalance(0);
        }
        grandchild->setBalance(0); // granchild is balances 
      }
    }
  }
}


#endif
