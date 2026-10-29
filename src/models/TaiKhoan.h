#ifndef TAIKHOAN_H
#define TAIKHOAN_H

#include <iostream>
#include <string>

using namespace std;

class TaiKhoan {
protected:
    string id;           // 6 chữ số: 111xxx (Admin), 222xxx (Khách hàng)
    string username;
    string passwordHash; // Chuỗi băm mật khẩu
    string vaiTro;       // "ADMIN" hoặc "CUSTOMER"

public:
    TaiKhoan()
        : id(""), username(""), passwordHash(""), vaiTro("") {}

    TaiKhoan(const string& id, const string& username,
             const string& passwordHash, const string& vaiTro)
        : id(id), username(username), passwordHash(passwordHash), vaiTro(vaiTro) {}

    virtual ~TaiKhoan() = default;

    // Getter
    const string& getId() const { return id; }
    const string& getUsername() const { return username; }
    const string& getPasswordHash() const { return passwordHash; }
    const string& getVaiTro() const { return vaiTro; }

    // Setter
    void setId(const string& v) { id = v; }
    void setUsername(const string& v) { username = v; }
    void setPasswordHash(const string& v) { passwordHash = v; }
    void setVaiTro(const string& v) { vaiTro = v; }

    // Nghiệp vụ
    bool kiemTraMatKhau(const string& hash) const {
        return passwordHash == hash;
    }

    bool isAdmin() const {
        return vaiTro == "ADMIN";
    }

    bool isCustomer() const {
        return vaiTro == "CUSTOMER";
    }

    // Đa hình
    virtual void show() const = 0;
    virtual void hienThiThongTin() const { show(); }
    virtual string xuatChuoiFile() const = 0;
};

#endif // TAIKHOAN_H

