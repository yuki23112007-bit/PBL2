#ifndef ORDERQUEUE_H
#define ORDERQUEUE_H

#include "../models/DonHang.h"
#include <string>
#include <vector>
#include <ctime>

using namespace std;

struct QueueNode {
    DonHang* data;
    QueueNode* next;

    QueueNode(DonHang* d, QueueNode* nxt = nullptr)
        : data(d), next(nxt) {}
};

struct PendingNode {
    DonHang* data;
    time_t thoiGianVaoPending;
    PendingNode* prev;
    PendingNode* next;

    PendingNode(DonHang* d, time_t tg, PendingNode* p = nullptr, PendingNode* n = nullptr)
        : data(d), thoiGianVaoPending(tg), prev(p), next(n) {}
};

class OrderQueue {
    private:
        // Hang doi chinh FIFO
        QueueNode* head;
        QueueNode* tail;
        int count;

        // Danh sach treo (Pending List - Doubly Linked List)
        PendingNode* pHead;
        PendingNode* pTail;
        int pendingCount;

        void xoaPendingNode(PendingNode* node);

    public:
        OrderQueue();
        ~OrderQueue();

        OrderQueue(const OrderQueue&) = delete;
        OrderQueue& operator=(const OrderQueue&) = delete;

        // Thao tac tren Hang doi chinh
        void enqueue(DonHang* dh);
        bool dequeue(DonHang*& dhOutput);
        bool peek(DonHang*& dhOutput) const;
        bool isEmpty() const;
        int getCount() const;

        // Thao tac tren Pending List (Chong nghen Head-of-Line Blocking)
        void themVaoPending(DonHang* dh, time_t tg = 0);
        bool tachSangDanhSachCho(const string& maDH);
        bool phucHoiDonCho(const string& maDH, DonHang*& dhOutput);
        vector<DonHang*> kiemTraVaHuyDonQuaHan5Phut(time_t thoiGianHienTai);
        int getPendingCount() const;

        // Tra cuu hien thi
        vector<DonHang*> layTatCaDonTrongQueue() const;
        vector<DonHang*> layTatCaDonPending() const;

        void clear();
};

#endif // ORDERQUEUE_H
