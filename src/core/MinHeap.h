#ifndef MINHEAP_H
#define MINHEAP_H

#include "../models/SanPham.h"
#include <vector>
#include <ctime>

using namespace std;

class MinHeap {
    private:
        vector<SanPham*> heap;

        int parent(int i) const { return (i - 1) / 2; }
        int leftChild(int i) const { return 2 * i + 1; }
        int rightChild(int i) const { return 2 * i + 2; }

        void upHeap(int index);
        void downHeap(int index);

    public:
        MinHeap();
        ~MinHeap();

        void them(SanPham* sp);
        SanPham* layDinh() const;
        SanPham* peek() const { return layDinh(); }
        bool trichXuatDinh(SanPham*& sp);
        bool pop(SanPham*& sp) { return trichXuatDinh(sp); }

        bool xoaTheoMa(const string& maSP);

        int getSize() const;
        bool isEmpty() const;
        void clear();

        vector<SanPham*> kiemTraSapHetHan(int soNgay = 2) const;
        const vector<SanPham*>& getHeapData() const { return heap; }
};

#endif
