#ifndef SELF_BALANCING_TREE_HPP
#define SELF_BALANCING_TREE_HPP

#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
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

            k _key;
            v _value;

            Node(k key, v value) : _key(key), _value(value) {}
            Node() : _key(k()), _value(v()) {}
        };

        size_t _size = 0;
        Node<K, V>* root = nullptr;

        void destroy_helper(Node<K, V>* N);

        inline Node<K, V>* minimum_leaf(Node<K, V>* X);
        inline void left_rotate(Node<K, V>* X);
        inline void right_rotate(Node<K, V>* X);
        inline void repair_tree_after_insert(Node<K, V>* Z);
        inline void repair_tree_after_delete(Node<K, V>* Z, Node<K, V>* P, bool left_child);
        inline void transplant(Node<K, V>* X, Node<K, V>* Y);
        inline void RB_BSTDelete(Node<K, V>* Z);

        void inorder_helper(Node<K,V>* N) {
            if (N == nullptr)
                return;

            inorder_helper(N->left_child);
            std::cout << N->_key << ", " << N->_value << std::endl;
            inorder_helper(N->right_child);
        }

        public:
        void inorder() {
            inorder_helper(root);
        }

        void bylevel() {
            if (_size == 0)
                return;
            std::vector<Node<K,V>*> level;
            level.push_back(root);

            while (!level.empty()) {
                std::vector<Node<K,V>*> next_level;
                for (auto N : level) {
                    std::cout << "(" << (N->parent ? N->parent->_key : 'N') << "->";
                    std::cout << N->_key << (N->color == NodeColor::Black ? 'B' : 'R') << ") ";
                    if (N->left_child) next_level.push_back(N->left_child);
                    if (N->right_child) next_level.push_back(N->right_child);
                }
                std::cout << std::endl;
                level = next_level;
            }
        }

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
        inline bool empty();
        inline size_t size();
    };

    template <typename K, typename V>
    inline self_balancing_tree<K, V>::Node<K, V>* self_balancing_tree<K, V>::minimum_leaf(Node<K, V>* X) {
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
    inline bool self_balancing_tree<K, V>::empty() {
        return (_size == 0);
    }

    template <typename K, typename V>
    V& self_balancing_tree<K, V>::operator[](K elem_key) {
        Node<K, V>* curr_node = root;
        Node<K, V>* prev_node = nullptr;

        while (curr_node != nullptr) {
            prev_node = curr_node;
            if (elem_key < curr_node->_key) {
                curr_node = curr_node->left_child;
            } else if (elem_key > curr_node->_key) {
                curr_node = curr_node->right_child;
            } else {
                return curr_node->_value;
            }
        }

        Node<K, V>* new_node = new Node<K, V>(elem_key, V());
        if (elem_key < prev_node->_key) {
            prev_node->left_child = new_node;
        } else {
            prev_node->right_child = new_node;
        }
        new_node->parent = prev_node;
        repair_tree_after_insert(new_node);
        _size++;

        return new_node->_value;
    }

    template <typename K, typename V>
    V& self_balancing_tree<K, V>::at(K elem_key) {
        Node<K, V>* curr_node = root;
        Node<K, V>* prev_node = nullptr;

        while (curr_node != nullptr) {
            prev_node = curr_node;
            if (elem_key < curr_node->_key) {
                curr_node = curr_node->left_child;
            } else if (elem_key > curr_node->_key) {
                curr_node = curr_node->right_child;
            } else {
                return curr_node->_value;
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
            if (elem_key < curr_node->_key) {
                curr_node = curr_node->left_child;
            } else if (elem_key > curr_node->_key) {
                curr_node = curr_node->right_child;
            } else {
                return curr_node->_value;
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
            if (curr_node->_key == k) {
                elem_found = true;
                break;
            } else if (k < curr_node->_key) {
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
            Z->_key = Y->_key;
            Z->_value = Y->_value;
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
                if (new_node->_key < curr_node->_key) {
                    curr_node = curr_node->left_child;
                } else if (new_node->_key > curr_node->_key) {
                    curr_node = curr_node->right_child;
                } else {
                    delete new_node;
                    return;
                }
            }
        
            new_node->parent = prev_node;

            if (new_node->_key < prev_node->_key) {
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
    void self_balancing_tree<K, V>::repair_tree_after_delete(Node<K, V>* Z, Node<K, V>* P, bool left_child) {
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
    void self_balancing_tree<K, V>::repair_tree_after_insert(Node<K, V>* Z) {
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