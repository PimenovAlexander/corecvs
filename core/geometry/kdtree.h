#ifndef KD_TREE
#define KD_TREE

#include <vector>
#include <memory>
#include <type_traits>
#include <iostream>
#include <thread>
#include <atomic>
#include "tbbwrapper/tbbWrapper.h"

namespace corecvs
{

/**
 *  KD Tree
 *    \param V - element type (vector with overloaded [] )
 *    \param N - dimention number
 *
 **/
template<typename V, int N>
class KDTree
{
    typedef typename std::remove_reference<decltype(V()[0])>::type InnerType;

    struct KDNode
    {
        V* item = NULL;
        KDNode *left  = NULL;
        KDNode *right = NULL;

        KDNode(V* v = nullptr) :
            item(v),
            left(nullptr),
            right(nullptr)
        {}

        bool isLeaf()
        {
            return (left == NULL) && (right == NULL);
        }

    };

    std::vector<V*> temporary;
    std::vector<V*> items;
    KDNode* root;
    std::vector<KDNode> nodes;
    std::vector<std::vector<V*>> references;
    int v, T;
    std::atomic<int> usedNodes;
public:
    void print(std::ostream& os) const
    {
        print(os, root, 0);
    }

    void print(std::ostream& os, KDNode *node, int depth) const
    {
        for (int i = 0; i < depth; ++i)
            os << "\t";
        int axis = depth % N;
        for (int i = 0; i < N; ++i)
            os << (i == axis ? "^" : "") << (*node->item)[i] << (i == axis ? "^" : "") << " ";
        os << std::endl;
        if (node->left)
            print(os, node->left, depth + 1);
        if (node->right)
            print(os, node->right, depth + 1);
    }

    template <typename _V, int _N>
    friend std::ostream& operator<<(std::ostream& o, const KDTree<_V, _N> &tree);

    KDTree(const std::vector<V*> &items, int T = 0) : items(items), root(nullptr), T(T), usedNodes(0)
    {
        createKDTree();
    }

    template <typename P>
    V* nearestNeighbour(const V& ref, const P& predicate)
    {
        if (!root)
            return nullptr;
        V* bestPivot = nullptr;
        v = 0;
        InnerType bestDistance = std::numeric_limits<InnerType>::max();
        //std::cout << bestDistance << std::flush;
        auto res  = nearestNeighbour(predicate, root, ref, bestPivot, bestDistance, 0);
        //std::cout << " (" << v << " / " << items.size() << " : " << double(v) / items.size() << ")" << std::endl;
        return res;
    }

    V* nearestNeighbour(const V& ref)
    {
        return nearestNeighbour(ref, [](V* /*q*/) { return true; });
    }

private:
    template <typename P>
    V* nearestNeighbour(const P& predicate, KDNode* node, const V& ref, V* &bestPivot, InnerType &bestDistance, int depth)
    {
        v++;
        if (predicate(node->item))
        {
            auto currDistance = computeDiff(*node->item, ref);
            if (bestDistance > currDistance)
            {
            //    std::cout << ">" << currDistance << std::flush;
                bestDistance = currDistance;
                bestPivot = node->item;
            }
        }

        auto axis = depth % N;
        auto minOther = ref[axis] - (*node->item)[axis];
        minOther *= minOther;

        auto dir =-cmp(node->item, &ref, axis);
        if (dir == 0)
            return node->item;
        //std::cout << "Pivot: " << *node->item << ", query: " << ref << ", depth = " << depth << " => minOther = " << minOther << std::endl;
        auto thisDir = dir < 0 ? node->left : node->right;
        auto otherDir= node->left + (node->right - thisDir);
        if (thisDir)
            nearestNeighbour(predicate, thisDir, ref, bestPivot, bestDistance, depth + 1);
        if (minOther < bestDistance && otherDir)
            nearestNeighbour(predicate, otherDir , ref, bestPivot, bestDistance, depth + 1);
        return bestPivot;
    }

    void createKDTree()
    {
        nodes.resize(items.size());
        references.resize(N, items);

        parallelable_for(0, N, [&](const BlockedRange<int> &r)
            {
                for (int i = r.begin(); i != r.end(); ++i)
                {
                    parallelable_sort(references[i].begin(), references[i].end(), [&](V* const a, V* const b)
                    {
                        auto res = cmp(a, b, i);
                        CORE_ASSERT_TRUE_S(res != 0);
                        return res  < 0;
                    });

                    int validRef = 0;
                    for (size_t j = 1; j < references[i].size(); j++)
                        if (cmp(references[i][validRef], references[i][j], i) != 0)
                            references[i][++validRef] = references[i][j];
                    references[i].resize(validRef + 1);
                }
            });
        temporary.resize(items.size());
        root = buildKDTree(0, (int)items.size() - 1, 0);
    }

    InnerType computeDiff(const V& a, const V &b)
    {
        InnerType dist = 0;
        for (int i = 0; i < N; ++i)
            dist += (a[i] - b[i]) * (a[i] - b[i]);
        return dist;
    }

    InnerType  cmp(const V* const a, const V* const b, int dim)
    {
        for (int j = 0; j < N; ++j)
        {
            int idx = (j + dim) % N;
            auto diff = (*a)[idx] - (*b)[idx];
            if (diff != 0)
                return diff;
        }
        return 0;
    }

    KDNode* buildKDTree(int start, int end, int depth)
    {
        int axis = depth % N;
        KDNode *node = &nodes[usedNodes++];
        if (end == start)
        {
            node->item = references[0][start];
            return node;
        }
        if (end == start + 1)
        {
            node->item = references[0][start];
            node->right = &nodes[usedNodes++];
            node->right->item = references[0][end];
            return node;
        }
        if (end == start + 2)
        {
            node->item = references[0][start + 1];
            node->right = &nodes[usedNodes++];
            node->right->item = references[0][end];
            node->left  = &nodes[usedNodes++];
            node->left->item = references[0][start];
            return node;
        }
        CORE_ASSERT_TRUE_S(end - start > 2);
        auto median = start + (end - start) / 2;
        node->item = references[0][median];

        for (int i = start; i <= end; ++i)
            temporary[i] = references[0][i];

        int lower, upper, preLower = -1, preUpper = -1;
        for (int i = 1; i < N; ++i)
        {
            lower = start - 1;
            upper = median;
            for (int j = start; j <= end; ++j)
            {
                if (references[i][j] == node->item)
                    continue;
                if (cmp(references[i][j], node->item, axis) < 0)
                    references[i - 1][++lower] = references[i][j];
                else
                    references[i - 1][++upper] = references[i][j];
            }

            CORE_ASSERT_TRUE_S(lower >= start && lower < median);
            CORE_ASSERT_TRUE_S(upper >  median&& upper <= end);
            CORE_ASSERT_TRUE_S(i == 1 || lower == preLower);
            CORE_ASSERT_TRUE_S(i == 1 || upper == preUpper);

            preLower = lower;
            preUpper = upper;
        }

        for (int i = start; i <= end; ++i)
            references[N - 1][i] = temporary[i];
        if (depth < T)
        {
            std::thread lhs([&]() {
                node->right = buildKDTree(median + 1, upper, depth + 1);
            });
                node->left  = buildKDTree(start,      lower, depth + 1);
            lhs.join();
        }
        else
        {
            node->left  = buildKDTree(start,      lower, depth + 1);
            node->right = buildKDTree(median + 1, upper, depth + 1);
        }
        return node;
    }
};
    template<typename V, int N>
    std::ostream& operator<<(std::ostream& o, const KDTree<V, N> &tree) { tree.print(o); return o; }
}

#endif
