#include "Trie.h"

using namespace std;

TrieNode::TrieNode(char c)
    : kyTu(c), firstChild(nullptr), nextSibling(nullptr),
      isEndOfWord(false), metadata(""), sanPhamRef(nullptr) {}

Trie::Trie() {
    root = new TrieNode('\0');
}

Trie::~Trie() {
    clear();
    delete root;
}

TrieNode* Trie::timCon(TrieNode* parent, char c) const {
    if (parent == nullptr) return nullptr;
    TrieNode* curr = parent->firstChild;
    while (curr != nullptr) {
        if (curr->kyTu == c) {
            return curr;
        }
        curr = curr->nextSibling;
    }
    return nullptr;
}

TrieNode* Trie::themCon(TrieNode* parent, char c) {
    if (parent == nullptr) return nullptr;

    TrieNode* existing = timCon(parent, c);
    if (existing != nullptr) {
        return existing;
    }

    TrieNode* newNode = new TrieNode(c);
    newNode->nextSibling = parent->firstChild;
    parent->firstChild = newNode;
    return newNode;
}

void Trie::chen(const string& tuKhoa, const string& meta, SanPham* sp) {
    if (tuKhoa.empty()) return;

    TrieNode* curr = root;
    for (char c : tuKhoa) {
        curr = themCon(curr, c);
    }

    curr->isEndOfWord = true;
    if (!meta.empty()) {
        curr->metadata = meta;
    }
    if (sp != nullptr) {
        curr->sanPhamRef = sp;
    }
}

TrieNode* Trie::timNode(const string& tuKhoa) const {
    if (tuKhoa.empty()) return root;

    TrieNode* curr = root;
    for (char c : tuKhoa) {
        curr = timCon(curr, c);
        if (curr == nullptr) {
            return nullptr;
        }
    }
    return curr;
}

bool Trie::tonTai(const string& tuKhoa) const {
    TrieNode* node = timNode(tuKhoa);
    return (node != nullptr && node->isEndOfWord);
}

void Trie::thuThapTuKhoa(TrieNode* node, string tuHienTai, vector<string>& kq) const {
    if (node == nullptr) return;

    if (node->isEndOfWord) {
        kq.push_back(tuHienTai);
    }

    TrieNode* child = node->firstChild;
    while (child != nullptr) {
        thuThapTuKhoa(child, tuHienTai + child->kyTu, kq);
        child = child->nextSibling;
    }
}

void Trie::thuThapSanPham(TrieNode* node, vector<SanPham*>& kq) const {
    if (node == nullptr) return;

    if (node->isEndOfWord && node->sanPhamRef != nullptr) {
        kq.push_back(node->sanPhamRef);
    }

    TrieNode* child = node->firstChild;
    while (child != nullptr) {
        thuThapSanPham(child, kq);
        child = child->nextSibling;
    }
}

vector<string> Trie::traCuuGoiY(const string& tienTo) const {
    vector<string> kq;
    TrieNode* startNode = timNode(tienTo);
    if (startNode != nullptr) {
        thuThapTuKhoa(startNode, tienTo, kq);
    }
    return kq;
}

vector<SanPham*> Trie::traCuuSanPham(const string& tienTo) const {
    vector<SanPham*> kq;
    TrieNode* startNode = timNode(tienTo);
    if (startNode != nullptr) {
        thuThapSanPham(startNode, kq);
    }
    return kq;
}

bool Trie::xoa(const string& tuKhoa) {
    if (tuKhoa.empty()) return false;
    TrieNode* node = timNode(tuKhoa);
    if (node != nullptr && node->isEndOfWord) {
        node->isEndOfWord = false;
        node->sanPhamRef = nullptr;
        node->metadata = "";
        return true;
    }
    return false;
}

void Trie::giaiPhongDeQuy(TrieNode* node) {
    if (node == nullptr) return;

    TrieNode* child = node->firstChild;
    while (child != nullptr) {
        TrieNode* next = child->nextSibling;
        giaiPhongDeQuy(child);
        delete child;
        child = next;
    }
    node->firstChild = nullptr;
}

void Trie::clear() {
    if (root != nullptr) {
        giaiPhongDeQuy(root);
        root->isEndOfWord = false;
        root->sanPhamRef = nullptr;
        root->metadata = "";
    }
}

bool Trie::isEmpty() const {
    return root == nullptr || root->firstChild == nullptr;
}
