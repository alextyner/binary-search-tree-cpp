/**
 * @file BinaryTree.cpp
 * @author Alex Tyner
 * A generic binary search tree map in C++17 but written like it's Java.
 */

#include <algorithm>
#include <memory>
#include <optional>
#include <sstream>
#include <vector>

#include <cassert>
#include <iostream>

template <class K, class V>
class BinarySearchTreeMap {
   private:
    class Node {
       private:
        K key;
        V value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

       public:
        Node(K key, V value, std::unique_ptr<Node> left, std::unique_ptr<Node> right) : key(key), value(value), left(std::move(left)), right(std::move(right)) {}
        Node(K key, V value) : Node(key, value, nullptr, nullptr) {}
        K getKey() {
            return key;
        }
        V getValue() {
            return value;
        }
        V setValue(V value) {
            V old = Node::value;
            Node::value = value;
            return old;
        }
        constexpr std::unique_ptr<Node> &getLeft() noexcept {  // & avoids copying the pointer around
            return left;
        }
        constexpr std::unique_ptr<Node> &getRight() noexcept {
            return right;
        }
    };

    size_t size_;
    std::unique_ptr<Node> root;

   public:
    BinarySearchTreeMap() : size_(0), root(nullptr) {}

    size_t size() {
        return size_;
    }

    std::optional<V> put(K const &key, V const &value) {
        if (!root)
            root = std::make_unique<Node>(key, value);
        else {
            auto ptr = &root;
            while (*ptr) {
                K current = (*ptr)->getKey();
                if (current == key) {
                    V ret = (*ptr)->getValue();
                    (*ptr)->setValue(value);
                    return ret;
                }
                if (key > current)
                    ptr = &(*ptr)->getRight();
                else
                    ptr = &(*ptr)->getLeft();
            }
            *ptr = std::make_unique<Node>(key, value);
        }
        ++size_;
        return std::nullopt;
    }

    std::optional<V> remove(K const &key) {
        auto ptr = &root;
        while (*ptr) {
            K current = (*ptr)->getKey();
            if (current == key) {
                if ((*ptr)->getRight() || (*ptr)->getLeft())
                    throw std::invalid_argument("ERROR: Only leaf nodes can be removed.");
                V ret = (*ptr)->getValue();
                *ptr = nullptr;
                --size_;
                return ret;
            }
            if (key > current)
                ptr = &(*ptr)->getRight();
            else
                ptr = &(*ptr)->getLeft();
        }
        return std::nullopt;
    }

    std::optional<V> get(K const &key) {
        auto ptr = &root;
        while (*ptr) {
            K current = (*ptr)->getKey();
            if (current == key) return (*ptr)->getValue();
            if (key > current)
                ptr = &(*ptr)->getRight();
            else
                ptr = &(*ptr)->getLeft();
        }
        return std::nullopt;
    }

    std::string toString() { // this isn't threadsafe, but lets me keep using unique_ptrs instead of shared_ptr/weak_ptr
        std::stringstream inOrder;
        Node *node = root.get();

        std::vector<Node *> stack;
        inOrder << "[ ";
        while (!stack.empty() || node) {
            if (node) {
                stack.push_back(node);
                node = node->getLeft().get();
            } else {
                node = stack.back();
                stack.pop_back();
                inOrder << "(" << node->getKey() << ", " << node->getValue()
                        << ") ";
                node = node->getRight().get();
            }
        }
        inOrder << "]";
        return inOrder.str();
    }

    bool isEmpty() { return size_ == 0; }
};

int main(int argc, char **argv) {
    BinarySearchTreeMap<int, std::string> tree;
    assert(tree.size() == 0);
    tree.put(4, "four");
    tree.put(5, "five");
    tree.put(3, "three");
    tree.put(1, "one");
    tree.put(6, "six");
    tree.put(0, "zero");
    tree.put(7, "seven");
    tree.put(2, "two");
    assert(tree.size() == 8);
    assert(tree.remove(2) == "two");
    assert(tree.size() == 7);
    assert(!tree.remove(8));
    assert(tree.size() == 7);
    try {
        tree.remove(4);
        assert(false);
    } catch (const std::exception &e) {
        // dynamic_cast tries to cast to std::invalid_argument, which inherits
        // from std::exception
        auto iae = dynamic_cast<std::invalid_argument const &>(e);  // throws std::bad_cast on failure when casting references
        assert(std::string(iae.what()) == "ERROR: Only leaf nodes can be removed.");
    }
    assert(tree.size() == 7);
    assert(tree.get(5) && tree.get(5) == "five");
    assert(!tree.get(8));
    std::cout << tree.toString() << std::endl;
    return 0;
}