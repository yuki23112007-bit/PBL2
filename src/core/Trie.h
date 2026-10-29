#ifndef TRIE_H
#define TRIE_H

#include "../models/SanPham.h"
#include <string>
#include <vector>

using namespace std;

struct TrieNode {
    char kyTu;
    TrieNode* firstChild;
    TrieNode* nextSibling;
    bool isEndOfWord;
    string metadata;
    SanPham* sanPhamRef; // Con tro muon tu HashTable, khong tu delete

    TrieNode(char c = '\0');
};

class Trie {
    private:
        TrieNode* root;

        TrieNode* timCon(TrieNode* parent, char c) const;
        TrieNode* themCon(TrieNode* parent, char c);
        bool xoaDeQuy(TrieNode* current, const string& tuKhoa, size_t index, bool& daXoa);
        void giaiPhongDeQuy(TrieNode* node);
        void thuThapTuKhoa(TrieNode* node, string tuHienTai, vector<string>& kq) const;
        void thuThapSanPham(TrieNode* node, vector<SanPham*>& kq) const;

    public:
        Trie();
        ~Trie();

        Trie(const Trie&) = delete;
        Trie& operator=(const Trie&) = delete;

        void chen(const string& tuKhoa, const string& meta = "", SanPham* sp = nullptr);
        bool xoa(const string& tuKhoa);
        bool tonTai(const string& tuKhoa) const;
        TrieNode* timNode(const string& tuKhoa) const;

        vector<string> traCuuGoiY(const string& tienTo) const;
        vector<SanPham*> traCuuSanPham(const string& tienTo) const;

        void clear();
        bool isEmpty() const;
};

#endif // TRIE_H
