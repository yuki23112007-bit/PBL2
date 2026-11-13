#include "HeThongQuanLy.h"
#include "../data/FileManager.h"

using namespace std;

void HeThongQuanLy::khoiDong() {
    FileManager fm("data");
    quanLyTaiKhoan.napTuFile(fm.docTaiKhoan());
    quanLyKhoNCC.napKho(fm.docKhoHang());
    quanLyKhoNCC.napNCC(fm.docNhaCungCap());
    quanLyDoanhThu.napDuLieu(fm.docDoanhThu());
}

void HeThongQuanLy::thoat() {
    FileManager fm("data");
    fm.ghiTaiKhoan(quanLyTaiKhoan.getDanhSachTaiKhoan());
    fm.ghiKhoHang(quanLyKhoNCC.xuatDanhSachKho());
    fm.ghiNhaCungCap(quanLyKhoNCC.xuatDanhSachNCC());
    fm.ghiDoanhThu(quanLyDoanhThu.getDanhSach());
}

QuanLyTaiKhoan& HeThongQuanLy::getQuanLyTaiKhoan() {
    return quanLyTaiKhoan;
}

QuanLyKhoNCC& HeThongQuanLy::getQuanLyKhoNCC() {
    return quanLyKhoNCC;
}

QuanLyDoanhThu& HeThongQuanLy::getQuanLyDoanhThu() {
    return quanLyDoanhThu;
}

