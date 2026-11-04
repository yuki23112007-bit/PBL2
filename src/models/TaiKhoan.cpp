#include "TaiKhoan.h"

using namespace std;

TaiKhoan::TaiKhoan()
    : id(""), username(""), passwordHash(""), vaiTro("") {}

TaiKhoan::TaiKhoan(const string& id, const string& username,
                   const string& passwordHash, const string& vaiTro)
    : id(id), username(username), passwordHash(passwordHash), vaiTro(vaiTro) {}

const string& TaiKhoan::getId() const {
    return id;
}

const string& TaiKhoan::getUsername() const {
    return username;
}

const string& TaiKhoan::getPasswordHash() const {
    return passwordHash;
}

const string& TaiKhoan::getVaiTro() const {
    return vaiTro;
}

void TaiKhoan::setId(const string& v) {
    id = v;
}

void TaiKhoan::setUsername(const string& v) {
    username = v;
}

void TaiKhoan::setPasswordHash(const string& v) {
    passwordHash = v;
}

void TaiKhoan::setVaiTro(const string& v) {
    vaiTro = v;
}

bool TaiKhoan::kiemTraMatKhau(const string& hash) const {
    return passwordHash == hash;
}

bool TaiKhoan::kiemTraDangNhap(const string& u, const string& pHash) const {
    return username == u && passwordHash == pHash;
}

bool TaiKhoan::isAdmin() const {
    return vaiTro == "ADMIN";
}

bool TaiKhoan::isCustomer() const {
    return vaiTro == "CUSTOMER";
}

void TaiKhoan::hienThiThongTin() const {
    show();
}

// =========================================================================
// ĐA NĂNG HÓA TOÁN TỬ (OPERATOR OVERLOADING)
// =========================================================================

bool TaiKhoan::operator==(const string& hash) const {
    return passwordHash == hash;
}

bool TaiKhoan::operator==(const TaiKhoan& other) const {
    return this->id == other.id && this->username == other.username;
}

bool TaiKhoan::operator!=(const TaiKhoan& other) const {
    return !(*this == other);
}

bool TaiKhoan::operator<(const TaiKhoan& other) const {
    return this->id < other.id;
}

ostream& operator<<(ostream& os, const TaiKhoan& tk) {
    os << tk.xuatChuoiFile();
    return os;
}
