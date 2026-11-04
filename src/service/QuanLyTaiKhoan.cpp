#include "QuanLyTaiKhoan.h"

using namespace std;

QuanLyTaiKhoan::~QuanLyTaiKhoan() {
    for (size_t i = 0; i < danhSachTaiKhoan.size(); i++) delete danhSachTaiKhoan[i];
    danhSachTaiKhoan.clear();
}

bool QuanLyTaiKhoan::dangKy(TaiKhoan* tk) {
    if (tk == nullptr || timTheoUsername(tk->getUsername()) != nullptr) return false;
    danhSachTaiKhoan.push_back(tk);
    return true;
}

TaiKhoan* QuanLyTaiKhoan::dangNhap(const string& username, const string& passwordHash) {
    TaiKhoan* tk = timTheoUsername(username);
    if (tk != nullptr && tk->kiemTraDangNhap(username, passwordHash)) return tk;
    return nullptr;
}

TaiKhoan* QuanLyTaiKhoan::timTheoUsername(const string& username) const {
    for (size_t i = 0; i < danhSachTaiKhoan.size(); i++) {
        if (danhSachTaiKhoan[i]->getUsername() == username) return danhSachTaiKhoan[i];
    }
    return nullptr;
}

bool QuanLyTaiKhoan::xoaTaiKhoan(const string& id) {
    for (size_t i = 0; i < danhSachTaiKhoan.size(); i++) {
        if (danhSachTaiKhoan[i]->getId() == id) {
            delete danhSachTaiKhoan[i];
            danhSachTaiKhoan.erase(danhSachTaiKhoan.begin() + i);
            return true;
        }
    }
    return false;
}

int QuanLyTaiKhoan::soLuongTaiKhoan() const {
    return (int)danhSachTaiKhoan.size();
}

