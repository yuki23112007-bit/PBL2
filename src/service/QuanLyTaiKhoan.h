#ifndef QUANLYTAIKHOAN_H
#define QUANLYTAIKHOAN_H

#include "../models/TaiKhoan.h"
#include "../models/Admin.h"
#include "../models/NguoiDung.h"
#include "../core/Trie.h"
#include <string>
#include <vector>

using namespace std;

// Nghiep vu dang ky, dang nhap, quan ly tai khoan (README muc 1.1, 2.1)
class QuanLyTaiKhoan {
private:
    vector<TaiKhoan*> danhSachTaiKhoan; // Lop nay so huu cac TaiKhoan*
    Trie cayTienToTaiKhoan;              // Cay tien to Trie luu tru username - O(L)

public:
    QuanLyTaiKhoan() = default;
    ~QuanLyTaiKhoan();

    QuanLyTaiKhoan(const QuanLyTaiKhoan&) = delete;
    QuanLyTaiKhoan& operator=(const QuanLyTaiKhoan&) = delete;

    bool dangKy(TaiKhoan* tk);                                              // false neu trung username (kiem tra bang Trie)
    TaiKhoan* dangNhap(const string& username, const string& passwordHash); // nullptr neu sai (tra cuu qua Trie O(L))
    TaiKhoan* timTheoUsername(const string& username) const;
    bool kiemTraUsernameTonTai(const string& username) const;               // Kiem tra bang cay Trie O(L)
    bool xoaTaiKhoan(const string& id);
    int soLuongTaiKhoan() const;
    Trie& getCayTienTo();
    const Trie& getCayTienTo() const;
    const vector<TaiKhoan*>& getDanhSachTaiKhoan() const;
};

#endif // QUANLYTAIKHOAN_H

