#ifndef SELF_BALANCING_TREE_HPP
#define SELF_BALANCING_TREE_HPP

#include <iostream>
#include <iterator>
#include <exception>

namespace MyDataStructures {
    template <typename K, typename V>
    class self_balancing_tree {
        enum class NodeColor {Red, Black};

        template <typename k, typename v>
        struct Node {
            NodeColor color = NodeColor::Red;

            Node* parent = nullptr;
            Node* left_child = nullptr;
            Node* right_child = nullptr;

            // This is used by the iterator class to return a
            // std::pair with a const K element, internally,
            // only K can be modified
            using intern_pair = std::pair<k, v>;
            using ext_pair    = std::pair<const k, v>;

            intern_pair key_val_pair;

            Node(k key, v value) { key_val_pair.first = key; key_val_pair.second = value; }
            Node() { }
        };

        class BSTIterator : 
            public std::iterator<std::bidirectional_iterator_tag, std::pair<const K, V>> {
        public:
            bool operator==(const BSTIterator& rhs) const {
                return this->curr_node == rhs.curr_node;
            }
            bool operator!=(const BSTIterator& rhs) const {
                return this->curr_node != rhs.curr_node;
            }

            const std::pair<const K, V>& operator*() const {
                return reinterpret_cast<const typename Node<K, V>::ext_pair&>(curr_node->key_val_pair);
            }

            const std::pair<const K, V>* operator->() const {
                return reinterpret_cast<const typename Node<K, V>::ext_pair*>(&curr_node->key_val_pair);
            }

            std::pair<const K, V>& operator*() {
                return reinterpret_cast<typename Node<K, V>::ext_pair&>(curr_node->key_val_pair);
            }

            std::pair<const K, V>* operator->() {
                return reinterpret_cast<typename Node<K, V>::ext_pair*>(&curr_node->key_val_pair);
            }

            BSTIterator& operator++() {
                Node<K, V>* N;

                // We're at the end? Let's check
                if (curr_node == nullptr) {
                    // Try to go back to begin(), so we start from root
                    curr_node = tree->root;

                    // The tree is empty, we can't allow this operation to succeed
                    if (curr_node == nullptr) {
                        throw std::underflow_error("");
                    }

                    // Get the minimum leaf
                    while (curr_node->left_child != nullptr) {
                        curr_node = curr_node->left_child;
                    }
                }
                // What if we're already reached the minimum? Check the 
                // inorder successor of this node if it exists 
                else if (curr_node->right_child != nullptr) {
                    curr_node = curr_node->right_child;

                    while (curr_node->left_child != nullptr) {
                        curr_node = curr_node->left_child;
                    }
                }
                // We've hit a leaf with no children, in that case
                // we have finished processing the left subtree,
                // So we go up each nodes' parents until we find a
                // node who is the left child of a parent, next
                // increment we will go down the right subtree or up 1 level 
                else {
                    N = curr_node->parent;
                    while (N != nullptr && curr_node == N->right_child) {
                        curr_node = N;
                        N = N->parent;
                    }

                    curr_node = N;
                }

                return *this;
            }

            BSTIterator operator++(int) {
                // Save the iterator before incrementing
                BSTIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            BSTIterator operator--() {
                Node<K, V>* N;

                // This is essentially the mirrored version of the
                // pre-increment operator
                
                // Again, we check if we're at end
                if (curr_node == nullptr) {
                    // Try to get the root
                    curr_node = tree->root;

                    // Cannot decrement on an empty tree
                    if (curr_node == nullptr) {
                        throw std::underflow_error("");
                    }

                    // This time find the maximum leaf
                    while (curr_node->right_child != nullptr) {
                        curr_node = curr_node->right_child;
                    }
                } 
                // We've finished processing the maxmimum node of this tree
                // Find the maxmimum node of the next subtree (inorder predecessor) 
                else if (curr_node->left_child != nullptr) {
                    curr_node = curr_node->left_child;

                    while (curr_node->right_child != nullptr) {
                        curr_node = curr_node->right_child;
                    }
                }
                // We've hit a leaf with no children, in that case
                // we have finished processing the right subtree,
                // So we go up each nodes' parents until we find a
                // node who is the right child of a parent, next
                // increment we will go down the left subtree or up 1 level  
                else {
                    N = curr_node->parent;
                    while (N != nullptr && curr_node == N->left_child) {
                        curr_node = N;
                        N = N->parent;
                    }

                    curr_node = N;
                }

                return *this;
            }

            BSTIterator operator--(int) {
                // Save the iterator before decrementing
                BSTIterator tmp = *this;
                --(*this);
                return tmp;
            }
        private:
            friend class self_balancing_tree<K, V>;

            Node<K, V> *curr_node;
            const self_balancing_tree<K, V> *tree;

            BSTIterator(Node<K, V>* N, const self_balancing_tree<K, V>* T) : curr_node(N), tree(T) {};
        };

        typedef BSTIterator iterator;
        typedef const BSTIterator const_iterator;

        size_t _size = 0;
        Node<K, V>* root = nullptr;

        void destroy_helper(Node<K, V>* N);

        inline Node<K, V>* minimum_leaf(Node<K, V>* X);
        inline const Node<K, V>* minimum_leaf(Node<K, V>* X) const;
        inline void left_rotate(Node<K, V>* X);
        inline void right_rotate(Node<K, V>* X);
        inline void repair_tree_after_insert(Node<K, V>* Z);
        inline void repair_tree_after_delete(Node<K, V>* Z, Node<K, V>* P, bool left_child);
        inline void transplant(Node<K, V>* X, Node<K, V>* Y);
        inline void RB_BSTDelete(Node<K, V>* Z);

        public:
        self_balancing_tree() {};
        ~self_balancing_tree() {
            destroy_helper(root);
            root = nullptr;
        }

        V& operator[](K elem_key);
        V& at(K elem_key);
        const V& at(K elem_key) const;
        
        inline void insert(K elem_key, V elem_value);
        inline void erase(const K& k);
        inline void clear();
        inline bool empty() const;
        inline size_t size();

        inline const_iterator find(const K& key) const;
        inline const_iterator cbegin() const;
        inline const_iterator cend() const;

        inline iterator find(const K& key);
        inline iterator begin();
        inline iterator end();
    };

    template <typename K, typename V>
    inline typename self_balancing_tree<K, V>::const_iterator self_balancing_tree<K, V>::find(const K& key) const {
        Node<K, V>* Z = root;

        while (Z != nullptr) {
            if (key == Z->key_val_pair.first) {
                break;
            } else if (key < Z->key_val_pair.first) {
                Z = Z->left_child;
            } else {
                Z = Z->right_child;
            }
        }

        return BSTIterator(Z, this);
    }

    template <typename K, typename V>
    inline typename self_balancing_tree<K, V>::iterator self_balancing_tree<K, V>::find(const K& key) {
        Node<K, V>* Z = root;

        while (Z != nullptr) {
            if (key == Z->key_val_pair.first) {
                break;
            } else if (key < Z->key_val_pair.first) {
                Z = Z->left_child;
            } else {
                Z = Z->right_child;
            }
        }

        return BSTIterator(Z, this);
    }

    template <typename K, typename V>
    inline typename self_balancing_tree<K, V>::const_iterator self_balancing_tree<K, V>::cbegin() const { 
        return BSTIterator(minimum_leaf(root), this);
    }

    template <typename K, typename V>
    inline typename self_balancing_tree<K, V>::const_iterator self_balancing_tree<K, V>::cend() const { 
        return BSTIterator(nullptr, this);
    }

    template <typename K, typename V>
    inline typename self_balancing_tree<K, V>::iterator self_balancing_tree<K, V>::begin() { 
        return BSTIterator(minimum_leaf(root), this);
    }

    template <typename K, typename V>
    inline typename self_balancing_tree<K, V>::iterator self_balancing_tree<K, V>::end() { 
        return BSTIterator(nullptr, this);
    }

    template <typename K, typename V>
    inline self_balancing_tree<K, V>::Node<K, V>* self_balancing_tree<K, V>::minimum_leaf(Node<K, V>* X) {
        while (X->left_child != nullptr) {
            X = X->left_child;
        }
        return X;
    }

    template <typename K, typename V>
    inline const typename self_balancing_tree<K, V>::Node<K, V>* self_balancing_tree<K, V>::minimum_leaf(Node<K, V>* X) const {
        while (X->left_child != nullptr) {
            X = X->left_child;
        }
        return X;
    }

    template <typename K, typename V>
    inline size_t self_balancing_tree<K, V>::size() {
        return _size;
    }

    template <typename K, typename V>
    inline bool self_balancing_tree<K, V>::empty() const {
        return (_size == 0);
    }

    template <typename K, typename V>
    V& self_balancing_tree<K, V>::operator[](K elem_key) {
        Node<K, V>* curr_node = root;
        Node<K, V>* prev_node = nullptr;

        while (curr_node != nullptr) {
            prev_node = curr_node;
            if (elem_key < curr_node->key_val_pair.first) {
                curr_node = curr_node->left_child;
            } else if (elem_key > curr_node->key_val_pair.first) {
                curr_node = curr_node->right_child;
            } else {
                return curr_node->key_val_pair.second;
            }
        }

        Node<K, V>* new_node = new Node<K, V>(elem_key, V());
        if (root == nullptr) {
            root = new_node;
        } else if (elem_key < prev_node->key_val_pair.first) {
            prev_node->left_child = new_node;
        } else {
            prev_node->right_child = new_node;
        }
        new_node->parent = prev_node;
        repair_tree_after_insert(new_node);
        _size++;

        return new_node->key_val_pair.second;
    }

    template <typename K, typename V>
    V& self_balancing_tree<K, V>::at(K elem_key) {
        Node<K, V>* curr_node = root;
        Node<K, V>* prev_node = nullptr;

        while (curr_node != nullptr) {
            prev_node = curr_node;
            if (elem_key < curr_node->key_val_pair.first) {
                curr_node = curr_node->left_child;
            } else if (elem_key > curr_node->key_val_pair.first) {
                curr_node = curr_node->right_child;
            } else {
                return curr_node->key_val_pair.second;
            }
        }

        throw std::out_of_range("Element not found in tree.");
    }

    template <typename K, typename V>
    const V& self_balancing_tree<K, V>::at(K elem_key) const {
        Node<K, V>* curr_node = root;
        Node<K, V>* prev_node = nullptr;

        while (curr_node != nullptr) {
            prev_node = curr_node;
            if (elem_key < curr_node->key_val_pair.first) {
                curr_node = curr_node->left_child;
            } else if (elem_key > curr_node->key_val_pair.first) {
                curr_node = curr_node->right_child;
            } else {
                return curr_node->key_val_pair.second;
            }
        }

        throw std::out_of_range("Element not found in tree.");
    }

    template <typename K, typename V>
    inline void self_balancing_tree<K, V>::transplant(Node<K, V>* X, Node<K, V>* Y) {
        if (X->parent == nullptr) {
            root = Y;
        } else if (X == X->parent->left_child) {
            X->parent->left_child = Y;
        } else {
            X->parent->right_child = Y;
        }
        if (Y != nullptr) Y->parent = X->parent;
    }

    template <typename K, typename V>
    inline void self_balancing_tree<K, V>::erase(const K& k) {
        Node<K, V>* curr_node = root;
        bool elem_found = false;

        while (curr_node != nullptr) {
            if (curr_node->key_val_pair.first == k) {
                elem_found = true;
                break;
            } else if (k < curr_node->key_val_pair.first) {
                curr_node = curr_node->left_child;
            } else {
                curr_node = curr_node->right_child;
            }
        }

        if (!elem_found) return;

        if (_size == 1) {
            delete curr_node;
            root = nullptr;
            _size--;
            return;
        }

        RB_BSTDelete(curr_node);
        _size--;
    }

    template <typename K, typename V>
    void self_balancing_tree<K, V>::destroy_helper(Node<K, V>* N) {
        if (N == nullptr)
            return;

        destroy_helper(N->left_child);
        destroy_helper(N->right_child);

        delete N;
    }

    template <typename K, typename V>
    inline void self_balancing_tree<K, V>::RB_BSTDelete(Node<K, V>* Z) {
        Node<K, V>* X;
        Node<K, V>* Y;
        Node<K, V>* P;
        bool left_child;
        
        if (Z->left_child == nullptr || Z->right_child == nullptr) {
            Y = Z;
        } else {
            Y = minimum_leaf(Z->right_child);
        }

        if (Y->left_child != nullptr) {
            X = Y->left_child;
        } else {
            X = Y->right_child;
        }

        P = Y->parent;

        if (Y->parent && Y == Y->parent->left_child) left_child = true;
        else left_child = false;

        transplant(Y, X);

        if (Y != Z) {
            Z->key_val_pair.first = Y->key_val_pair.first;
            Z->key_val_pair.second = Y->key_val_pair.second;
        }

        if (Y->color == NodeColor::Black) {
            repair_tree_after_delete(X, P, left_child);
        }

        delete Y;
    }

    template <typename K, typename V>
    inline void self_balancing_tree<K, V>::clear() {
        destroy_helper(root);
        root = nullptr;
        _size = 0;
    }

    template <typename K, typename V>
    inline void self_balancing_tree<K, V>::insert(K elem_key, V elem_value) {
        Node<K, V>* new_node = new Node<K, V>(elem_key, elem_value);
        Node<K, V>* curr_node = root;
        Node<K, V>* prev_node = nullptr;

        if (root != nullptr) {
            while (curr_node != nullptr) {
                prev_node = curr_node;
                if (new_node->key_val_pair.first < curr_node->key_val_pair.first) {
                    curr_node = curr_node->left_child;
                } else if (new_node->key_val_pair.first > curr_node->key_val_pair.first) {
                    curr_node = curr_node->right_child;
                } else {
                    delete new_node;
                    return;
                }
            }
        
            new_node->parent = prev_node;

            if (new_node->key_val_pair.first < prev_node->key_val_pair.first) {
                prev_node->left_child = new_node;
            } else {
                prev_node->right_child = new_node;
            }
        } else {
            root = new_node;
        }

        _size++;
        repair_tree_after_insert(new_node);
    }

    template <typename K, typename V>
    inline void self_balancing_tree<K, V>::left_rotate(Node<K, V>* X) {
        Node<K, V>* Y = X->right_child;
        X->right_child = Y->left_child;
        if (Y->left_child != nullptr) {
            Y->left_child->parent = X;
        }
        Y->parent = X->parent;
        if (X->parent == nullptr) {
            root = Y;
        } else if (X == X->parent->left_child) {
            X->parent->left_child = Y;
        } else {
            X->parent->right_child = Y;
        }
        Y->left_child = X;
        X->parent = Y;
    }

    template <typename K, typename V>
    inline void self_balancing_tree<K, V>::right_rotate(Node<K, V>* X) {
        Node<K, V>* Y = X->left_child;
        X->left_child = Y->right_child;
        if (Y->right_child != nullptr) {
            Y->right_child->parent = X;
        }
        Y->parent = X->parent;
        if (X->parent == nullptr) {
            root = Y;
        } else if (X == X->parent->right_child) {
            X->parent->right_child = Y;
        } else {
            X->parent->left_child = Y;
        }
        Y->right_child = X;
        X->parent = Y;
    }

    template <typename K, typename V>
    inline void self_balancing_tree<K, V>::repair_tree_after_delete(Node<K, V>* Z, Node<K, V>* P, bool left_child) {
        Node<K, V>* W;
        while (Z != root && (Z == nullptr || Z->color == NodeColor::Black)) {
            if (left_child) {
                W = P->right_child;
                if (W != nullptr && W->color == NodeColor::Red) {
                    W->color = NodeColor::Black;
                    P->color = NodeColor::Red;
                    left_rotate(P);
                    W = P->right_child;
                }

                bool is_left_black = W->left_child == nullptr || W->left_child->color == NodeColor::Black;
                bool is_right_black = W->right_child == nullptr || W->right_child->color == NodeColor::Black;

                if (is_left_black && is_right_black) {
                    W->color = NodeColor::Red;
                    Z = P;
                    P = P->parent;
                    left_child = (P != nullptr && P->left_child == Z);
                } else {
                    if (is_right_black) {
                        W->left_child->color = NodeColor::Black;
                        W->color = NodeColor::Red;
                        right_rotate(W);
                        W = P->right_child;
                    }

                    W->color = P->color;
                    P->color = NodeColor::Black;
                    if (W->right_child != nullptr) {
                        W->right_child->color = NodeColor::Black;
                    }
                    left_rotate(P);
                    Z = root;
                }
            } else {
                W = P->left_child;
                if (W != nullptr && W->color == NodeColor::Red) {
                    W->color = NodeColor::Black;
                    P->color = NodeColor::Red;
                    right_rotate(P);
                    W = P->left_child;
                }

                bool is_left_black = W->left_child == nullptr || W->left_child->color == NodeColor::Black;
                bool is_right_black = W->right_child == nullptr || W->right_child->color == NodeColor::Black;


                if (is_left_black && is_right_black) {
                    W->color = NodeColor::Red;
                    Z = P;
                    P = P->parent;
                    left_child = (P != nullptr && P->left_child == Z);
                } else {
                    if (is_left_black) {
                        W->right_child->color = NodeColor::Black;
                        W->color = NodeColor::Red;
                        left_rotate(W);
                        W = P->left_child;
                    }

                    W->color = P->color;
                    P->color = NodeColor::Black;
                    if (W->left_child != nullptr) {
                        W->left_child->color = NodeColor::Black;
                    }
                    right_rotate(P);
                    Z = root;
                }
            }
        }

        Z->color = NodeColor::Black;
    }

    template <typename K, typename V>
    inline void self_balancing_tree<K, V>::repair_tree_after_insert(Node<K, V>* Z) {
        while (Z != root && Z->parent->color == NodeColor::Red) {
            Node<K, V>* Y; 
            if (Z->parent == Z->parent->parent->left_child) {
                Y = Z->parent->parent->right_child;

                if (Y != nullptr && Y->color == NodeColor::Red) {
                    Z->parent->color = NodeColor::Black;
                    Y->color = NodeColor::Black;
                    Z->parent->parent->color = NodeColor::Red;
                    Z = Z->parent->parent;
                } else {
                    if (Z == Z->parent->right_child) {
                        Z = Z->parent;
                        left_rotate(Z);
                    }
                    Z->parent->color = NodeColor::Black;
                    Z->parent->parent->color = NodeColor::Red;
                    right_rotate(Z->parent->parent);
                }

            } else {
                Y = Z->parent->parent->left_child;

                if (Y != nullptr && Y->color == NodeColor::Red) {
                    Z->parent->color = NodeColor::Black;
                    Y->color = NodeColor::Black;
                    Z->parent->parent->color = NodeColor::Red;
                    Z = Z->parent->parent;
                } else {
                    if (Z == Z->parent->left_child) {
                        Z = Z->parent;
                        right_rotate(Z);
                    }
                    Z->parent->color = NodeColor::Black;
                    Z->parent->parent->color = NodeColor::Red;
                    left_rotate(Z->parent->parent);
                }

            }
        }
        root->color = NodeColor::Black;
    }
};

#endif