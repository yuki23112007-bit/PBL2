#ifndef MINHEAP_H
#define MINHEAP_H

#include "../models/ThucPham.h"
#include <vector>
#include <ctime>

using namespace std;

class MinHeap {
    private:
        vector<ThucPham*> heap;

        int parent(int i) const { return (i - 1) / 2; }
        int leftChild(int i) const { return 2 * i + 1; }
        int rightChild(int i) const { return 2 * i + 2; }

        void upHeap(int index);
        void downHeap(int index);

    public:
        MinHeap();
        ~MinHeap();

        void them(ThucPham* sp);
        ThucPham* layDinh() const;
        ThucPham* peek() const { return layDinh(); }
        bool trichXuatDinh(ThucPham*& sp);
        bool pop(ThucPham*& sp) { return trichXuatDinh(sp); }

        bool xoaTheoMa(const string& maSP);

        int getSize() const;
        bool isEmpty() const;
        void clear();

        vector<ThucPham*> kiemTraSapHetHan(int soNgay = 2) const;
        const vector<ThucPham*>& getHeapData() const { return heap; }
};

#endif // MINHEAP_H
