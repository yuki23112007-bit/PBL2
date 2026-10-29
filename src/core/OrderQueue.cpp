#include "OrderQueue.h"

using namespace std;

OrderQueue::OrderQueue()
    : head(nullptr), tail(nullptr), count(0),
      pHead(nullptr), pTail(nullptr), pendingCount(0) {}

OrderQueue::~OrderQueue() {
    clear();
}

void OrderQueue::enqueue(DonHang* dh) {
    if (dh == nullptr) return;

    QueueNode* newNode = new QueueNode(dh);
    if (tail == nullptr) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    count++;
}

bool OrderQueue::dequeue(DonHang*& dhOutput) {
    if (head == nullptr) {
        dhOutput = nullptr;
        return false;
    }

    QueueNode* temp = head;
    dhOutput = temp->data;
    head = head->next;
    if (head == nullptr) {
        tail = nullptr;
    }

    delete temp;
    count--;
    return true;
}

bool OrderQueue::peek(DonHang*& dhOutput) const {
    if (head == nullptr) {
        dhOutput = nullptr;
        return false;
    }
    dhOutput = head->data;
    return true;
}

bool OrderQueue::isEmpty() const {
    return head == nullptr;
}

int OrderQueue::getCount() const {
    return count;
}

int OrderQueue::getPendingCount() const {
    return pendingCount;
}

void OrderQueue::themVaoPending(DonHang* dh, time_t tg) {
    if (dh == nullptr) return;

    time_t vaoLuc = (tg == 0) ? time(nullptr) : tg;
    PendingNode* newNode = new PendingNode(dh, vaoLuc);

    if (pTail == nullptr) {
        pHead = pTail = newNode;
    } else {
        pTail->next = newNode;
        newNode->prev = pTail;
        pTail = newNode;
    }
    pendingCount++;
}

void OrderQueue::xoaPendingNode(PendingNode* node) {
    if (node == nullptr) return;

    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        pHead = node->next;
    }

    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        pTail = node->prev;
    }

    delete node;
    pendingCount--;
}

bool OrderQueue::tachSangDanhSachCho(const string& maDH) {
    if (maDH.empty() || head == nullptr) return false;

    // Neu don nam o dau Queue
    if (head->data->getMaDonHang() == maDH) {
        DonHang* dh = nullptr;
        dequeue(dh);
        dh->setTrangThaiDonHang("Dang Cho Hang");
        themVaoPending(dh);
        return true;
    }

    // Neu don nam o giua Queue
    QueueNode* prev = head;
    QueueNode* curr = head->next;
    while (curr != nullptr) {
        if (curr->data->getMaDonHang() == maDH) {
            prev->next = curr->next;
            if (curr == tail) {
                tail = prev;
            }
            DonHang* dh = curr->data;
            delete curr;
            count--;

            dh->setTrangThaiDonHang("Dang Cho Hang");
            themVaoPending(dh);
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    return false;
}

bool OrderQueue::phucHoiDonCho(const string& maDH, DonHang*& dhOutput) {
    PendingNode* curr = pHead;
    while (curr != nullptr) {
        if (curr->data->getMaDonHang() == maDH) {
            dhOutput = curr->data;
            dhOutput->setTrangThaiDonHang("Cho Duyet");
            xoaPendingNode(curr);
            return true;
        }
        curr = curr->next;
    }

    dhOutput = nullptr;
    return false;
}

vector<DonHang*> OrderQueue::kiemTraVaHuyDonQuaHan5Phut(time_t thoiGianHienTai) {
    vector<DonHang*> danhSachBiHuy;
    PendingNode* curr = pHead;

    while (curr != nullptr) {
        PendingNode* nextNode = curr->next;
        if ((thoiGianHienTai - curr->thoiGianVaoPending) >= 300) { // 5 phut = 300 giay
            curr->data->setTrangThaiDonHang("Da huy do qua han");
            danhSachBiHuy.push_back(curr->data);
            xoaPendingNode(curr);
        }
        curr = nextNode;
    }

    return danhSachBiHuy;
}

vector<DonHang*> OrderQueue::layTatCaDonTrongQueue() const {
    vector<DonHang*> danhSach;
    QueueNode* curr = head;
    while (curr != nullptr) {
        if (curr->data != nullptr) {
            danhSach.push_back(curr->data);
        }
        curr = curr->next;
    }
    return danhSach;
}

vector<DonHang*> OrderQueue::layTatCaDonPending() const {
    vector<DonHang*> danhSach;
    PendingNode* curr = pHead;
    while (curr != nullptr) {
        if (curr->data != nullptr) {
            danhSach.push_back(curr->data);
        }
        curr = curr->next;
    }
    return danhSach;
}

void OrderQueue::clear() {
    while (head != nullptr) {
        QueueNode* temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    count = 0;

    while (pHead != nullptr) {
        PendingNode* temp = pHead;
        pHead = pHead->next;
        delete temp;
    }
    pTail = nullptr;
    pendingCount = 0;
}
