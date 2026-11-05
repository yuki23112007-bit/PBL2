#include "QuanLyDoanhThu.h"
#include <iostream>

using namespace std;

int QuanLyDoanhThu::timViTriTheoNgay(const string& ngay) const {
    for (size_t i = 0; i < danhSachDoanhThu.size(); i++) {
        if (danhSachDoanhThu[i].getNgayThongKe() == ngay) return (int)i;
    }
    return -1;
}

void QuanLyDoanhThu::ghiNhanDonHang(const string& ngay, double giaTriDonHang) {
    int viTri = timViTriTheoNgay(ngay);
    if (viTri < 0) {
        danhSachDoanhThu.push_back(DoanhThuNgay(ngay));
        viTri = (int)danhSachDoanhThu.size() - 1;
    }
    danhSachDoanhThu[viTri].congDonDoanhThu(giaTriDonHang);
}

const DoanhThuNgay* QuanLyDoanhThu::layDoanhThuNgay(const string& ngay) const {
    int viTri = timViTriTheoNgay(ngay);
    return viTri < 0 ? nullptr : &danhSachDoanhThu[viTri];
}

double QuanLyDoanhThu::tongDoanhThu() const {
    double tong = 0.0;
    for (size_t i = 0; i < danhSachDoanhThu.size(); i++) tong += danhSachDoanhThu[i].getTongDoanhThu();
    return tong;
}

int QuanLyDoanhThu::soNgayCoDoanhThu() const {
    return (int)danhSachDoanhThu.size();
}

void QuanLyDoanhThu::hienThiBaoCao() const {
    for (size_t i = 0; i < danhSachDoanhThu.size(); i++) danhSachDoanhThu[i].hienThiThongTin();
    cout << "Tong doanh thu: " << tongDoanhThu() << endl;
}

