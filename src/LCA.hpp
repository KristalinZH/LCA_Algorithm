#ifndef LCA_HPP
#define LCA_HPP

#include <utility>
#include <sstream>
#include "Tree.hpp"
#include "PlusMinusOneRMQ.hpp"

template<typename T>
class LCA {
    public:
        LCA(const Tree<T>* tree) : root(tree) {
            const size_t size = root -> size();

            E.resize(2*size - 1);
            std::vector<size_t> D(2*size - 1);

            size_t index = 0;
            EulerTraversal(root, E, D, 0, index);

            RMQ = PlusMinusOneRMQ(D);
        }

        const Tree<T>* getLCA(const Tree<T>* u, const Tree<T>* v) const {
            if(u == nullptr || v == nullptr) {
                throw std::runtime_error("Nullptr passed as argument!");
            }

            const size_t indexU = getEdgeIndex(u);
            const size_t indexV = getEdgeIndex(v);

            if(indexU == E.size() || indexV == E.size()) {
                std::ostringstream ostr;
                ostr << *root;
                throw std::runtime_error("Node not found in this tree: " + ostr.str());
            }

            return E[RMQ.getRMQ(indexU, indexV)];
        }

    private:
        const Tree<T>* root;
        std::vector<const Tree<T>*> E;
        PlusMinusOneRMQ RMQ;

        void EulerTraversal(
            const Tree<T>* tree, 
            std::vector<const Tree<T>*>& edges, 
            std::vector<size_t>& depths,
            const size_t depth,
            size_t& index) {

            edges[index] = tree;
            depths[index] = depth;

            const std::list<Tree<T>*>& children = tree -> children();
            for(const Tree<T>* child : children) {
                index++;
                EulerTraversal(child, edges, depths, depth + 1, index);
                index++;
                edges[index] = tree;
                depths[index] = depth;
            }
        }

        size_t getEdgeIndex(const Tree<T>* edge) const {
            const size_t size = E.size();
            size_t edgeIndex = size;

            for(size_t i = 0; i < size; i++) {
                if(E[i] == edge) {
                    edgeIndex = i;
                    break;
                }
            }

            return edgeIndex;
        }
};

#endif