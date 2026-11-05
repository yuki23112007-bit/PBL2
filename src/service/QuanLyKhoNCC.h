#ifndef QUANLYKHONCC_H
#define QUANLYKHONCC_H

#include "../models/KhoHang.h"
#include "../models/NhaCungCap.h"
#include <string>
#include <vector>

using namespace std;

// Nghiep vu kho hang va nha cung cap (README muc 1.3, 1.6)
class QuanLyKhoNCC {
private:
    vector<KhoHang*> danhSachKho;        // Lop nay so huu KhoHang*
    vector<NhaCungCap*> danhSachNCC;     // Lop nay so huu NhaCungCap*

public:
    QuanLyKhoNCC() = default;
    ~QuanLyKhoNCC();

    QuanLyKhoNCC(const QuanLyKhoNCC&) = delete;
    QuanLyKhoNCC& operator=(const QuanLyKhoNCC&) = delete;

    bool themKho(KhoHang* kho);
    KhoHang* timKhoTheoMa(const string& maKho) const;

    bool themNhaCungCap(NhaCungCap* ncc);
    NhaCungCap* timNCCTheoMa(const string& maNCC) const;

    int soLuongKho() const;
    int soLuongNCC() const;
};

#endif // QUANLYKHONCC_H

