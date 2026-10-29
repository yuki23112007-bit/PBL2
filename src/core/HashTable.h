#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "../models/SanPham.h"
#include <string>
#include <vector>

using namespace std;

struct HashNode {
    string key;      // maSanPham
    SanPham* value;       // Pointer to SanPham
    HashNode* next;       // Chaining

    HashNode(const string& k, SanPham* val, HashNode* nxt = nullptr)
        : key(k), value(val), next(nxt) {}
};

class HashTable {
    private:
        int capacity;
        int size;
        HashNode** table;

    public:
        HashTable(int cap = 101);
        ~HashTable();

        // Cam sao chep ngau nhien vi HashTable quan ly bo nho con tro SanPham*
        HashTable(const HashTable&) = delete;
        HashTable& operator=(const HashTable&) = delete;

        int hashFunction(const string& key) const;

        bool them(const string& key, SanPham* sp);
        SanPham* timKiem(const string& key) const;
        bool capNhat(const string& key, SanPham* spMoi);
        bool xoa(const string& key);

        vector<SanPham*> layTatCaConTro() const;

        int getSize() const;
        int getCapacity() const;
        bool isEmpty() const;
        void clear();
};

#endif
