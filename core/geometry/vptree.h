#ifndef VPTREE_H
#define VPTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <queue>
#include <iostream>
#include <limits>

const int NUMBER_ITEMS_IN_LEAF = 10;
const int NUMBER_OF_INSERTS_TO_REBUILD = 2e5;

template<typename T>
struct TreeNode {
	int index;
	double threshold;
	TreeNode<T>* left;
	TreeNode<T>* right;
	bool isLeaf;
	std::vector <std::pair<T, int> > leafItems;

	TreeNode() :
		index(0), threshold(0.), left(0), right(0), isLeaf(false) {}

	~TreeNode() {
		delete left;
		delete right;
	}
};

struct HeapItem {
	HeapItem(int index, double dist) : index(index), dist(dist) {}
	int index;
	double dist;
	bool operator<(const HeapItem& o) const {
		return dist < o.dist;
	}
};

template<typename T>
struct DistanceComparator {
	T& item;
	DistanceComparator(T& item) : item(item) {}

	bool operator()(const T& a, const T& b) {
		return item.dist(a) < item.dist(b);
	}
};


template<typename T>
class VpTree {
public:
	VpTree();

	~VpTree();

	void create(const std::vector<T> & items);

	void insert(const T newItem);

	void search(const T& target, int k, std::vector<T>* results, std::vector<double>* distances);

private:
	std::vector<T> _items;
	double _tau;
	int count_inserts = 0;
	TreeNode<T>* _root;

	TreeNode<T>* insert_(TreeNode<T> *TreeNode, std::pair<T, int> newItem);

	TreeNode<T>* buildFromPoints(int lower, int upper);

	void search_(TreeNode<T>* TreeNode, const T& target, int k, std::priority_queue<HeapItem>& heap);
};



template<typename T>
VpTree<T>::VpTree() : _root(0) {}

template<typename T>
VpTree<T>::~VpTree() { delete _root; }

template<typename T>
void VpTree<T>::create(const std::vector<T> & items) {
	delete _root;
	_items = items;
	_root = buildFromPoints(0, items.size());
}

template<typename T>
void VpTree<T>::insert(const T newItem) {
	_items.push_back(newItem);
	count_inserts++;
	if (count_inserts > NUMBER_OF_INSERTS_TO_REBUILD) {
		count_inserts = 0;
		create(_items);
		return;
	}
	_root = insert_(_root, { newItem, _items.size() - 1 });
}

template<typename T>
void VpTree<T>::search(const T& target, int k, std::vector<T>* results, std::vector<double>* distances) {
	std::priority_queue<HeapItem> heap;

	_tau = std::numeric_limits<double>::max();
	search_(_root, target, k, heap);

	results->clear(); distances->clear();

	while (!heap.empty()) {
		results->push_back(_items[heap.top().index]);
		distances->push_back(heap.top().dist);
		heap.pop();
	}

	std::reverse(results->begin(), results->end());
	std::reverse(distances->begin(), distances->end());
}

template<typename T>
TreeNode<T>* VpTree<T>::insert_(TreeNode<T> *node, std::pair<T, int> newItem) {
	if (node == NULL) {
		TreeNode<T>* newNode = new TreeNode<T>();
		newNode->isLeaf = true;
		newNode->leafItems.clear();
		newNode->leafItems.push_back(newItem);
		return newNode;
	}

	if (node->isLeaf == true) {
		node->leafItems.push_back(newItem);
		if (node->leafItems.size() == NUMBER_ITEMS_IN_LEAF) {
			TreeNode<T> *newNode = new TreeNode<T>();
			int index = node->leafItems[rand() % node->leafItems.size()].second;
			newNode->index = index;
			sort(node->leafItems.begin(), node->leafItems.end(),
				[&](std::pair<T, int> a, std::pair<T, int> b) {
				return (_items[index].dist(a.first) < _items[index].dist(b.first));
			});

			int size = node->leafItems.size();

			newNode->threshold = (_items[index].dist(node->leafItems[size / 2].first));

			TreeNode<T> *leftNode = new TreeNode<T>();
			leftNode->isLeaf = true;
			leftNode->leafItems.clear();
			for (int i = 0; i < size / 2; i++) {
				leftNode->leafItems.push_back(node->leafItems[i]);
			}

			TreeNode<T> *rightNode = new TreeNode<T>();
			rightNode->isLeaf = true;
			rightNode->leafItems.clear();
			for (int i = size / 2; i < size; i++) {
				rightNode->leafItems.push_back(node->leafItems[i]);
			}

			newNode->left = leftNode;
			newNode->right = rightNode;
			delete(node);
			return newNode;
		}
		return node;
	}


	double dist = _items[node->index].dist(newItem.first);
	if (dist < node->threshold) {
		node->left = insert_(node->left, newItem);
	}
	else {
		node->right = insert_(node->right, newItem);
	}

	return node;
}

template<typename T>
TreeNode<T>* VpTree<T>::buildFromPoints(int lower, int upper) {
	if (upper == lower) {
		return NULL;
	}

	TreeNode<T>* node = new TreeNode<T>();
	node->index = lower;

	if (upper - lower > NUMBER_ITEMS_IN_LEAF) {

		int i = (int)((double)rand() / RAND_MAX * (upper - lower - 1)) + lower;
		std::swap(_items[lower], _items[i]);

		int median = (upper + lower) / 2;

		std::nth_element(
			_items.begin() + lower + 1,
			_items.begin() + median,
			_items.begin() + upper,
			DistanceComparator<T>(_items[lower]));

		node->threshold = _items[lower].dist(_items[median]);

		node->index = lower;
		node->left = buildFromPoints(lower + 1, median);
		node->right = buildFromPoints(median, upper);
	}
	else {
		node->isLeaf = true;
		node->leafItems.clear();
		for (int i = lower; i < upper; i++) {
			node->leafItems.push_back({ _items[i], i });
		}

	}

	return node;
}

template<typename T>
void VpTree<T>::search_(TreeNode<T>* node, const T& target, int k, std::priority_queue<HeapItem>& heap) {
	if (node == NULL) return;

	if (node->isLeaf) {
		for (int i = 0; i < node->leafItems.size(); i++) {
			double dist = node->leafItems[i].first.dist(target);

			if (dist < _tau) {
				if (heap.size() == k) heap.pop();
				heap.push(HeapItem(node->leafItems[i].second, dist));
				if (heap.size() == k) _tau = heap.top().dist;
			}
		}

		return;
	}

	double dist = _items[node->index].dist(target);

	if (dist < _tau) {
		if (heap.size() == k) heap.pop();
		heap.push(HeapItem(node->index, dist));
		if (heap.size() == k) _tau = heap.top().dist;
	}

	if (node->left == NULL && node->right == NULL) {
		return;
	}

	if (dist < node->threshold) {
		if (dist - _tau <= node->threshold) {
			search_(node->left, target, k, heap);
		}

		if (dist + _tau >= node->threshold) {
			search_(node->right, target, k, heap);
		}

	}
	else {
		if (dist + _tau >= node->threshold) {
			search_(node->right, target, k, heap);
		}

		if (dist - _tau <= node->threshold) {
			search_(node->left, target, k, heap);
		}
	}
}


#endif // !VPTREE_H
