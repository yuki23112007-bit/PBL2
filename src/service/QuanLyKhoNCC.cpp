#include "QuanLyKhoNCC.h"

using namespace std;

QuanLyKhoNCC::~QuanLyKhoNCC() {
    for (size_t i = 0; i < danhSachKho.size(); i++) delete danhSachKho[i];
    for (size_t i = 0; i < danhSachNCC.size(); i++) delete danhSachNCC[i];
    danhSachKho.clear();
    danhSachNCC.clear();
}

bool QuanLyKhoNCC::themKho(KhoHang* kho) {
    if (kho == nullptr || timKhoTheoMa(kho->getMaKho()) != nullptr) return false;
    danhSachKho.push_back(kho);
    return true;
}

KhoHang* QuanLyKhoNCC::timKhoTheoMa(const string& maKho) const {
    for (size_t i = 0; i < danhSachKho.size(); i++) {
        if (danhSachKho[i]->getMaKho() == maKho) return danhSachKho[i];
    }
    return nullptr;
}

bool QuanLyKhoNCC::themNhaCungCap(NhaCungCap* ncc) {
    if (ncc == nullptr || timNCCTheoMa(ncc->getMaNhaCungCap()) != nullptr) return false;
    danhSachNCC.push_back(ncc);
    return true;
}

NhaCungCap* QuanLyKhoNCC::timNCCTheoMa(const string& maNCC) const {
    for (size_t i = 0; i < danhSachNCC.size(); i++) {
        if (danhSachNCC[i]->getMaNhaCungCap() == maNCC) return danhSachNCC[i];
    }
    return nullptr;
}

int QuanLyKhoNCC::soLuongKho() const {
    return (int)danhSachKho.size();
}

int QuanLyKhoNCC::soLuongNCC() const {
    return (int)danhSachNCC.size();
}

