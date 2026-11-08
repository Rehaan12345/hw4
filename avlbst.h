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
    void rotateLeft(AVLNode<Key,Value>* x);
    void rotateRight(AVLNode<Key,Value>* x);
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void removeFix(AVLNode<Key,Value>* n, int diff);

};

// balance = height(right) - height(left)

// x's right child moves up into x's spot, x becomes its left child
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* x)
{
    AVLNode<Key,Value>* y = x->getRight();
    AVLNode<Key,Value>* p = x->getParent();

    x->setRight(y->getLeft());
    if(y->getLeft() != NULL) y->getLeft()->setParent(x);

    y->setParent(p);
    if(p == NULL) this->root_ = y;
    else if(p->getLeft() == x) p->setLeft(y);
    else p->setRight(y);

    y->setLeft(x);
    x->setParent(y);
}

// x's left child moves up into x's spot, x becomes its right child
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* x)
{
    AVLNode<Key,Value>* y = x->getLeft();
    AVLNode<Key,Value>* p = x->getParent();

    x->setLeft(y->getRight());
    if(y->getRight() != NULL) y->getRight()->setParent(x);

    y->setParent(p);
    if(p == NULL) this->root_ = y;
    else if(p->getLeft() == x) p->setLeft(y);
    else p->setRight(y);

    y->setRight(x);
    x->setParent(y);
}

// p's subtree just got taller (n is p's child), fix up toward the root
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
{
    if(p == NULL) return;
    AVLNode<Key,Value>* g = p->getParent();
    if(g == NULL) return;

    if(p == g->getLeft()) {
        g->updateBalance(-1);
        if(g->getBalance() == 0) return;
        if(g->getBalance() == -1) {
            insertFix(g, p);
            return;
        }
        // g->getBalance() == -2
        if(n == p->getLeft()) {
            // zig-zig
            rotateRight(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {
            // zig-zag
            rotateLeft(p);
            rotateRight(g);
            if(n->getBalance() == -1) { p->setBalance(0); g->setBalance(1); }
            else if(n->getBalance() == 0) { p->setBalance(0); g->setBalance(0); }
            else { p->setBalance(-1); g->setBalance(0); }
            n->setBalance(0);
        }
    }
    else {
        g->updateBalance(1);
        if(g->getBalance() == 0) return;
        if(g->getBalance() == 1) {
            insertFix(g, p);
            return;
        }
        // g->getBalance() == 2
        if(n == p->getRight()) {
            // zig-zig
            rotateLeft(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {
            // zig-zag
            rotateRight(p);
            rotateLeft(g);
            if(n->getBalance() == 1) { p->setBalance(0); g->setBalance(-1); }
            else if(n->getBalance() == 0) { p->setBalance(0); g->setBalance(0); }
            else { p->setBalance(1); g->setBalance(0); }
            n->setBalance(0);
        }
    }
}

// n's balance must change by diff (+1: left side shrank, -1: right side shrank)
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int diff)
{
    if(n == NULL) return;

    // compute next step before any rotation changes n's parent
    AVLNode<Key,Value>* p = n->getParent();
    int ndiff = 0;
    if(p != NULL) ndiff = (n == p->getLeft()) ? 1 : -1;

    if(diff == -1) {
        if(n->getBalance() + diff == -2) {
            AVLNode<Key,Value>* c = n->getLeft();
            if(c->getBalance() == -1) {
                // zig-zig, height shrinks so keep going
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if(c->getBalance() == 0) {
                // zig-zig, height unchanged so stop
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
            }
            else {
                // zig-zag
                AVLNode<Key,Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                if(g->getBalance() == 1) { n->setBalance(0); c->setBalance(-1); }
                else if(g->getBalance() == 0) { n->setBalance(0); c->setBalance(0); }
                else { n->setBalance(1); c->setBalance(0); }
                g->setBalance(0);
                removeFix(p, ndiff);
            }
        }
        else if(n->getBalance() + diff == -1) {
            n->setBalance(-1);
        }
        else {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else {
        if(n->getBalance() + diff == 2) {
            AVLNode<Key,Value>* c = n->getRight();
            if(c->getBalance() == 1) {
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if(c->getBalance() == 0) {
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            else {
                AVLNode<Key,Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if(g->getBalance() == -1) { n->setBalance(0); c->setBalance(1); }
                else if(g->getBalance() == 0) { n->setBalance(0); c->setBalance(0); }
                else { n->setBalance(-1); c->setBalance(0); }
                g->setBalance(0);
                removeFix(p, ndiff);
            }
        }
        else if(n->getBalance() + diff == 1) {
            n->setBalance(1);
        }
        else {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // case 1: empty tree
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key,Value>(new_item.first, new_item.second, NULL);
        return;
    }

    // walk down like a normal BST insert
    AVLNode<Key,Value>* parent = static_cast<AVLNode<Key,Value>*>(this->root_);
    AVLNode<Key,Value>* n = NULL;
    while(n == NULL) {
        if(new_item.first == parent->getKey()) {
            parent->setValue(new_item.second);
            return;
        }
        else if(new_item.first < parent->getKey()) {
            if(parent->getLeft() == NULL) {
                n = new AVLNode<Key,Value>(new_item.first, new_item.second, parent);
                parent->setLeft(n);
            }
            else parent = parent->getLeft();
        }
        else {
            if(parent->getRight() == NULL) {
                n = new AVLNode<Key,Value>(new_item.first, new_item.second, parent);
                parent->setRight(n);
            }
            else parent = parent->getRight();
        }
    }

    // parent was leaning, now even: height unchanged
    if(parent->getBalance() != 0) {
        parent->setBalance(0);
        return;
    }
    parent->updateBalance(n == parent->getLeft() ? -1 : 1);
    insertFix(parent, n);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key,Value>* n = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
    if(n == NULL) return;

    // two children: swap with predecessor so n has at most one child
    if(n->getLeft() != NULL && n->getRight() != NULL) {
        AVLNode<Key,Value>* pred = static_cast<AVLNode<Key,Value>*>(this->predecessor(n));
        nodeSwap(n, pred);
    }

    AVLNode<Key,Value>* child = n->getLeft();
    if(child == NULL) child = n->getRight();
    AVLNode<Key,Value>* parent = n->getParent();

    // removing a left child makes parent lean right (+1), and vice versa
    int diff = 0;
    if(parent != NULL) diff = (n == parent->getLeft()) ? 1 : -1;

    // promote the child into n's spot
    if(child != NULL) child->setParent(parent);
    if(parent == NULL) this->root_ = child;
    else if(parent->getLeft() == n) parent->setLeft(child);
    else parent->setRight(child);
    delete n;

    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
