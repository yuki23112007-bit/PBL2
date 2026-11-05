#ifndef HETHONGQUANLY_H
#define HETHONGQUANLY_H

#include "../service/QuanLyTaiKhoan.h"
#include "../service/QuanLyKhoNCC.h"
#include "../service/QuanLyDoanhThu.h"

using namespace std;

// Facade: dieu phoi cac lop service cho WebServer (README muc 2.1)
class HeThongQuanLy {
private:
    QuanLyTaiKhoan quanLyTaiKhoan;
    QuanLyKhoNCC quanLyKhoNCC;
    QuanLyDoanhThu quanLyDoanhThu;

public:
    HeThongQuanLy() = default;
    ~HeThongQuanLy() = default;

    HeThongQuanLy(const HeThongQuanLy&) = delete;
    HeThongQuanLy& operator=(const HeThongQuanLy&) = delete;

    void khoiDong(); // Nap du lieu khi khoi dong (se noi voi FileManager)
    void thoat();    // Luu du lieu khi thoat

    QuanLyTaiKhoan& getQuanLyTaiKhoan();
    QuanLyKhoNCC& getQuanLyKhoNCC();
    QuanLyDoanhThu& getQuanLyDoanhThu();
};

#endif // HETHONGQUANLY_H

