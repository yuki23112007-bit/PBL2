#include "HashTable.h"

using namespace std;

HashTable::HashTable(int cap)
    : capacity(cap > 0 ? cap : 101), size(0) {
    table = new HashNode*[capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i] = nullptr;
    }
}

HashTable::~HashTable() {
    clear();
    delete[] table;
}

int HashTable::hashFunction(const string& key) const {
    // djb2 hash function
    unsigned long hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c); /* hash * 33 + c */
    }
    return static_cast<int>(hash % capacity);
}

bool HashTable::them(const string& key, SanPham* sp) {
    if (key.empty() || sp == nullptr) {
        return false;
    }

    int idx = hashFunction(key);
    HashNode* curr = table[idx];

    // Kiem tra trung key
    while (curr != nullptr) {
        if (curr->key == key) {
            return false; // Da ton tai
        }
        curr = curr->next;
    }

    // Them vao dau danh sach lien ket (Chaining)
    HashNode* newNode = new HashNode(key, sp, table[idx]);
    table[idx] = newNode;
    size++;
    return true;
}

SanPham* HashTable::timKiem(const string& key) const {
    if (key.empty()) {
        return nullptr;
    }

    int idx = hashFunction(key);
    HashNode* curr = table[idx];

    while (curr != nullptr) {
        if (curr->key == key) {
            return curr->value;
        }
        curr = curr->next;
    }

    return nullptr;
}

bool HashTable::capNhat(const string& key, SanPham* spMoi) {
    if (key.empty() || spMoi == nullptr) {
        return false;
    }

    int idx = hashFunction(key);
    HashNode* curr = table[idx];

    while (curr != nullptr) {
        if (curr->key == key) {
            if (curr->value != spMoi) {
                delete curr->value; // Giai phong SanPham cu
                curr->value = spMoi;
            }
            return true;
        }
        curr = curr->next;
    }

    return false;
}

bool HashTable::xoa(const string& key) {
    if (key.empty()) {
        return false;
    }

    int idx = hashFunction(key);
    HashNode* curr = table[idx];
    HashNode* prev = nullptr;

    while (curr != nullptr) {
        if (curr->key == key) {
            if (prev == nullptr) {
                table[idx] = curr->next;
            } else {
                prev->next = curr->next;
            }

            delete curr->value; // Giai phong SanPham duoc quan ly boi HashTable
            delete curr;        // Giai phong HashNode
            size--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    return false;
}

vector<SanPham*> HashTable::layTatCaConTro() const {
    vector<SanPham*> danhSach;
    danhSach.reserve(size);

    for (int i = 0; i < capacity; ++i) {
        HashNode* curr = table[i];
        while (curr != nullptr) {
            if (curr->value != nullptr) {
                danhSach.push_back(curr->value);
            }
            curr = curr->next;
        }
    }

    return danhSach;
}

int HashTable::getSize() const {
    return size;
}

int HashTable::getCapacity() const {
    return capacity;
}

bool HashTable::isEmpty() const {
    return size == 0;
}

void HashTable::clear() {
    for (int i = 0; i < capacity; ++i) {
        HashNode* curr = table[i];
        while (curr != nullptr) {
            HashNode* temp = curr;
            curr = curr->next;
            delete temp->value; // Giai phong SanPham
            delete temp;        // Giai phong Node
        }
        table[i] = nullptr;
    }
    size = 0;
}
