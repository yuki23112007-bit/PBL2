#include "MinHeap.h"

using namespace std;

static void hoanDoi(SanPham*& a, SanPham*& b) {
    SanPham* temp = a;
    a = b;
    b = temp;
}

MinHeap::MinHeap() {}

MinHeap::~MinHeap() {
    clear();
}

void MinHeap::upHeap(int index) {
    while (index > 0) {
        int p = parent(index);
        if (heap[index]->ngayhethang() < heap[p]->ngayhethang()) {
            hoanDoi(heap[index], heap[p]);
            index = p;
        } else {
            break;
        }
    }
}

void MinHeap::downHeap(int index) {
    int n = static_cast<int>(heap.size());
    while (true) {
        int smallest = index;
        int left = leftChild(index);
        int right = rightChild(index);

        if (left < n && heap[left]->ngayhethang() < heap[smallest]->ngayhethang()) {
            smallest = left;
        }
        if (right < n && heap[right]->ngayhethang() < heap[smallest]->ngayhethang()) {
            smallest = right;
        }

        if (smallest != index) {
            hoanDoi(heap[index], heap[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

void MinHeap::them(SanPham* sp) {
    if (sp == nullptr) return;
    heap.push_back(sp);
    upHeap(static_cast<int>(heap.size()) - 1);
}

SanPham* MinHeap::layDinh() const {
    if (heap.empty()) return nullptr;
    return heap[0];
}

bool MinHeap::trichXuatDinh(SanPham*& sp) {
    if (heap.empty()) {
        sp = nullptr;
        return false;
    }

    sp = heap[0];
    if (heap.size() == 1) {
        heap.pop_back();
    } else {
        heap[0] = heap.back();
        heap.pop_back();
        downHeap(0);
    }
    return true;
}

bool MinHeap::xoaTheoMa(const string& maSP) {
    int idx = -1;
    for (size_t i = 0; i < heap.size(); ++i) {
        if (heap[i]->getMaSP() == maSP) {
            idx = static_cast<int>(i);
            break;
        }
    }

    if (idx == -1) return false;

    if (idx == static_cast<int>(heap.size()) - 1) {
        heap.pop_back();
    } else {
        heap[idx] = heap.back();
        heap.pop_back();
        downHeap(idx);
        upHeap(idx);
    }
    return true;
}

int MinHeap::getSize() const {
    return static_cast<int>(heap.size());
}

bool MinHeap::isEmpty() const {
    return heap.empty();
}

void MinHeap::clear() {
    heap.clear();
}

vector<SanPham*> MinHeap::kiemTraSapHetHan(int soNgay) const {
    vector<SanPham*> danhSach;
    time_t now = time(nullptr);
    time_t nguongGiay = static_cast<time_t>(soNgay) * 86400;

    for (SanPham* sp : heap) {
        if (sp != nullptr) {
            time_t thoiGianConLai = sp->ngayhethang() - now;
            if (thoiGianConLai <= nguongGiay) {
                danhSach.push_back(sp);
            }
        }
    }

    return danhSach;
}
