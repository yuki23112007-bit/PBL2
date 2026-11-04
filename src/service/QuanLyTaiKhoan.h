#ifndef QUANLYTAIKHOAN_H
#define QUANLYTAIKHOAN_H

#include "../models/TaiKhoan.h"
#include "../models/Admin.h"
#include "../models/NguoiDung.h"
#include <string>
#include <vector>

using namespace std;

// Nghiep vu dang ky, dang nhap, quan ly tai khoan (README muc 1.1, 2.1)
class QuanLyTaiKhoan {
private:
    vector<TaiKhoan*> danhSachTaiKhoan; // Lop nay so huu cac TaiKhoan*

public:
    QuanLyTaiKhoan() = default;
    ~QuanLyTaiKhoan();

    QuanLyTaiKhoan(const QuanLyTaiKhoan&) = delete;
    QuanLyTaiKhoan& operator=(const QuanLyTaiKhoan&) = delete;

    bool dangKy(TaiKhoan* tk);                                          // false neu trung username
    TaiKhoan* dangNhap(const string& username, const string& passwordHash); // nullptr neu sai
    TaiKhoan* timTheoUsername(const string& username) const;
    bool xoaTaiKhoan(const string& id);
    int soLuongTaiKhoan() const;
};

#endif // QUANLYTAIKHOAN_H

