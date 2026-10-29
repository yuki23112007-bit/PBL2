#include "BinarySearchTree.h"

using namespace std;

BinarySearchTree::BinarySearchTree()
    : root(nullptr), nodeCount(0) {}

BinarySearchTree::~BinarySearchTree() {
    clear();
}

BSTNode* BinarySearchTree::chenDeQuy(BSTNode* node, const string& key, DonHang* dh) {
    if (node == nullptr) {
        nodeCount++;
        return new BSTNode(key, dh);
    }

    if (key < node->key) {
        node->left = chenDeQuy(node->left, key, dh);
    } else if (key > node->key) {
        node->right = chenDeQuy(node->right, key, dh);
    } else {
        // Neu da ton tai, cap nhat con tro data
        node->data = dh;
    }

    return node;
}

void BinarySearchTree::chen(const string& key, DonHang* dh) {
    if (key.empty() || dh == nullptr) return;
    root = chenDeQuy(root, key, dh);
}

BSTNode* BinarySearchTree::timKiemDeQuy(BSTNode* node, const string& key) const {
    if (node == nullptr || node->key == key) {
        return node;
    }

    if (key < node->key) {
        return timKiemDeQuy(node->left, key);
    }
    return timKiemDeQuy(node->right, key);
}

DonHang* BinarySearchTree::timKiem(const string& key) const {
    if (key.empty()) return nullptr;
    BSTNode* res = timKiemDeQuy(root, key);
    return res ? res->data : nullptr;
}

BSTNode* BinarySearchTree::timMinNode(BSTNode* node) const {
    BSTNode* curr = node;
    while (curr && curr->left != nullptr) {
        curr = curr->left;
    }
    return curr;
}

BSTNode* BinarySearchTree::xoaDeQuy(BSTNode* node, const string& key, bool& daXoa) {
    if (node == nullptr) return nullptr;

    if (key < node->key) {
        node->left = xoaDeQuy(node->left, key, daXoa);
    } else if (key > node->key) {
        node->right = xoaDeQuy(node->right, key, daXoa);
    } else {
        // Tim thay node can xoa
        daXoa = true;
        if (node->left == nullptr) {
            BSTNode* temp = node->right;
            delete node;
            nodeCount--;
            return temp;
        } else if (node->right == nullptr) {
            BSTNode* temp = node->left;
            delete node;
            nodeCount--;
            return temp;
        }

        // Node co 2 con: tim successor nho nhat o nhanh phai
        BSTNode* temp = timMinNode(node->right);
        node->key = temp->key;
        node->data = temp->data;
        node->right = xoaDeQuy(node->right, temp->key, daXoa);
    }

    return node;
}

bool BinarySearchTree::xoa(const string& key) {
    if (key.empty() || root == nullptr) return false;
    bool daXoa = false;
    root = xoaDeQuy(root, key, daXoa);
    return daXoa;
}

void BinarySearchTree::duyetInOrderDeQuy(BSTNode* node, vector<DonHang*>& kq) const {
    if (node == nullptr) return;
    duyetInOrderDeQuy(node->left, kq);
    if (node->data != nullptr) {
        kq.push_back(node->data);
    }
    duyetInOrderDeQuy(node->right, kq);
}

vector<DonHang*> BinarySearchTree::layDanhSachSapXep() const {
    vector<DonHang*> kq;
    kq.reserve(nodeCount);
    duyetInOrderDeQuy(root, kq);
    return kq;
}

int BinarySearchTree::count() const {
    return nodeCount;
}

bool BinarySearchTree::isEmpty() const {
    return root == nullptr;
}

void BinarySearchTree::giaiPhongDeQuy(BSTNode* node) {
    if (node == nullptr) return;
    giaiPhongDeQuy(node->left);
    giaiPhongDeQuy(node->right);
    delete node;
}

void BinarySearchTree::clear() {
    giaiPhongDeQuy(root);
    root = nullptr;
    nodeCount = 0;
}
