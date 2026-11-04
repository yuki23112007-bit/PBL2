#ifndef QUANLYDOANHTHU_H
#define QUANLYDOANHTHU_H

#include "../models/DoanhThuNgay.h"
#include <string>
#include <vector>

using namespace std;

// Ghi nhan va bao cao doanh thu (README muc 1.5, 1.7)
class QuanLyDoanhThu {
private:
    vector<DoanhThuNgay> danhSachDoanhThu;

    int timViTriTheoNgay(const string& ngay) const;

public:
    QuanLyDoanhThu() = default;
    ~QuanLyDoanhThu() = default;

    // Ghi nhan 1 don hang hoan thanh vao ngay (YYYY-MM-DD)
    void ghiNhanDonHang(const string& ngay, double giaTriDonHang);

    const DoanhThuNgay* layDoanhThuNgay(const string& ngay) const; // nullptr neu chua co
    double tongDoanhThu() const;
    int soNgayCoDoanhThu() const;
    void hienThiBaoCao() const;
};

#endif // QUANLYDOANHTHU_H

