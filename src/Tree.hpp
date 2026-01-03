#ifndef TREE_HPP
#define TREE_HPP

#include <iostream>
#include <list>
#include <cstddef>

template<typename T>
class Tree {
    public:
        Tree(const T& _data) : data(_data) {}
        
        Tree(const T& _data, const std::list<Tree<T>*>& _subtrees) : data(_data) {
            for(const Tree<T>* subtree : _subtrees) {
                subtrees.push_back(new Tree<T>(*subtree));
            }
        }

        Tree(const Tree<T>& other) : data(other.data) {
            for(const Tree<T>* subtree : other.subtrees) {
                subtrees.push_back(new Tree<T>(*subtree));
            }
        }

        Tree& operator=(const Tree<T>& other) {
            if(this!= &other) {
                data = other.data;
                erase();
                for(const Tree<T>* subtree : other.subtrees) {
                    subtrees.push_back(new Tree<T>(*subtree));
                }
            }

            return *this;
        }

        Tree(Tree<T>&& other) : data(std::move(other.data)) {
            subtrees = std::move(other.subtrees);
            other.subtrees.clear();
        }


        Tree& operator=(Tree<T>&& other) {
            if(this!= &other) {
                data = std::move(other.data);
                erase();
                subtrees = std::move(other.subtrees);
                other.subtrees.clear();
            }

            return *this;
        }

        ~Tree() {
            erase();
        }

        void addSubtree(Tree* tree) {
            subtrees.push_back(tree);
        }
        
        const T& root() const {
            return data;
        }

        T& root() {
            return data;
        }
        
        const std::list<Tree<T>*>& children() const {
            return subtrees;
        }

        size_t size() const {
            size_t count = 1;

            for(const auto& child : subtrees) {
                count += child -> size();
            }

            return count;
        }

        bool search(const T& node) const {
            if(data == node) {
                return true;
            }

            for(const auto* child : subtrees) {
                if(child -> search(node)) {
                    return true;
                }
            }

            return false;
        }

        bool search(const Tree& node) const {
            if(*this == node) {
                return true;
            }

            for(const Tree<T>* child : subtrees) {
                if(child -> search(node)) {
                    return true;
                }
            }

            return false;
        }

        bool operator==(const Tree& other) const {
            if(data != other.data) {
                return false;
            }

            if(subtrees.size() != other.subtrees.size()) {
                return false;
            }

            auto it = subtrees.begin();
            auto otherIt = other.subtrees.begin();
        
            while(it != subtrees.end()) {
                if(*it != *otherIt) {
                    return false;
                }
                it++;
                otherIt++;
            }

            return true;
        }

        bool operator!=(const Tree& other) const {
            return !(*this == other);
        }

        friend std::ostream& operator<<(std::ostream& os, const Tree& tree) {
            os << "(" << tree.data;
            for (const auto& child : tree.subtrees)
                os << " " << *child;
            os << ")";
            return os;
        }
    private:
        T data;
        std::list<Tree<T>*> subtrees;

        void erase() {
            for(Tree<T>* node : subtrees) {
                delete node;
            }
            subtrees.clear();
        }
};

#endif