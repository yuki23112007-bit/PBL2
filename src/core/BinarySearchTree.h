#ifndef BINARYSEARCHTREE_H
#define BINARYSEARCHTREE_H

#include "../models/DonHang.h"
#include <string>
#include <vector>

using namespace std;

struct BSTNode {
    string key; // maDonHang
    DonHang* data;
    BSTNode* left;
    BSTNode* right;

    BSTNode(const string& k, DonHang* d)
        : key(k), data(d), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
    private:
        BSTNode* root;
        int nodeCount;

        BSTNode* chenDeQuy(BSTNode* node, const string& key, DonHang* dh);
        BSTNode* timKiemDeQuy(BSTNode* node, const string& key) const;
        BSTNode* xoaDeQuy(BSTNode* node, const string& key, bool& daXoa);
        BSTNode* timMinNode(BSTNode* node) const;
        void duyetInOrderDeQuy(BSTNode* node, vector<DonHang*>& kq) const;
        void giaiPhongDeQuy(BSTNode* node);

    public:
        BinarySearchTree();
        ~BinarySearchTree();

        BinarySearchTree(const BinarySearchTree&) = delete;
        BinarySearchTree& operator=(const BinarySearchTree&) = delete;

        void chen(const string& key, DonHang* dh);
        DonHang* timKiem(const string& key) const;
        bool xoa(const string& key);

        vector<DonHang*> layDanhSachSapXep() const;

        int count() const;
        bool isEmpty() const;
        void clear();
};

#endif // BINARYSEARCHTREE_H
