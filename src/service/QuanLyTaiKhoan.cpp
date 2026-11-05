#include "QuanLyTaiKhoan.h"

using namespace std;

QuanLyTaiKhoan::~QuanLyTaiKhoan() {
    for (size_t i = 0; i < danhSachTaiKhoan.size(); i++) delete danhSachTaiKhoan[i];
    danhSachTaiKhoan.clear();
    cayTienToTaiKhoan.clear();
}

bool QuanLyTaiKhoan::dangKy(TaiKhoan* tk) {
    if (tk == nullptr) return false;
    // Kiem tra bang cay tien to Trie voi do phuc tap O(L) de chan trung username
    if (cayTienToTaiKhoan.tonTai(tk->getUsername())) return false;

    danhSachTaiKhoan.push_back(tk);
    cayTienToTaiKhoan.chen(tk->getUsername(), tk->getVaiTro() + "|" + tk->getId());
    return true;
}

TaiKhoan* QuanLyTaiKhoan::dangNhap(const string& username, const string& passwordHash) {
    // Kiem tra su ton tai cua username bang Trie O(L) truoc
    if (!cayTienToTaiKhoan.tonTai(username)) return nullptr;

    TaiKhoan* tk = timTheoUsername(username);
    if (tk != nullptr && tk->kiemTraDangNhap(username, passwordHash)) return tk;
    return nullptr;
}

bool QuanLyTaiKhoan::kiemTraUsernameTonTai(const string& username) const {
    return cayTienToTaiKhoan.tonTai(username);
}

TaiKhoan* QuanLyTaiKhoan::timTheoUsername(const string& username) const {
    if (!cayTienToTaiKhoan.tonTai(username)) return nullptr;
    for (size_t i = 0; i < danhSachTaiKhoan.size(); i++) {
        if (danhSachTaiKhoan[i]->getUsername() == username) return danhSachTaiKhoan[i];
    }
    return nullptr;
}

bool QuanLyTaiKhoan::xoaTaiKhoan(const string& id) {
    for (size_t i = 0; i < danhSachTaiKhoan.size(); i++) {
        if (danhSachTaiKhoan[i]->getId() == id) {
            cayTienToTaiKhoan.xoa(danhSachTaiKhoan[i]->getUsername());
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

Trie& QuanLyTaiKhoan::getCayTienTo() {
    return cayTienToTaiKhoan;
}

const Trie& QuanLyTaiKhoan::getCayTienTo() const {
    return cayTienToTaiKhoan;
}

const vector<TaiKhoan*>& QuanLyTaiKhoan::getDanhSachTaiKhoan() const {
    return danhSachTaiKhoan;
}

