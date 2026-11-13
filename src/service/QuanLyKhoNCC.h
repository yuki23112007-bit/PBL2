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
    bool luuHoacCapNhatKho(const string& maKho, const string& viTri, int sucChua, const string& maSP);
    bool xoaKho(const string& maKho);
    void napKho(const vector<KhoHang>& ds);
    vector<KhoHang> xuatDanhSachKho() const;
    const vector<KhoHang*>& getDanhSachKho() const;

    bool themNhaCungCap(NhaCungCap* ncc);
    NhaCungCap* timNCCTheoMa(const string& maNCC) const;
    bool luuHoacCapNhatNCC(const string& maNCC, const string& tenCT, const string& diaChi, const string& sdt, const string& danhMuc);
    bool xoaNCC(const string& maNCC);
    void napNCC(const vector<NhaCungCap>& ds);
    vector<NhaCungCap> xuatDanhSachNCC() const;
    const vector<NhaCungCap*>& getDanhSachNCC() const;

    void clear();
    int soLuongKho() const;
    int soLuongNCC() const;
};

#endif // QUANLYKHONCC_H

