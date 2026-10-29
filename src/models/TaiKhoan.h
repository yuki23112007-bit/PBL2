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
    TaiKhoan();
    TaiKhoan(const string& id, const string& username,
             const string& passwordHash, const string& vaiTro);
    virtual ~TaiKhoan() = default;

    // Getter
    const string& getId() const;
    const string& getUsername() const;
    const string& getPasswordHash() const;
    const string& getVaiTro() const;

    // Setter
    void setId(const string& v);
    void setUsername(const string& v);
    void setPasswordHash(const string& v);
    void setVaiTro(const string& v);

    // Nghiệp vụ
    bool kiemTraMatKhau(const string& hash) const;
    bool isAdmin() const;
    bool isCustomer() const;

    // Đa hình
    virtual void show() const = 0;
    virtual void hienThiThongTin() const;
    virtual string xuatChuoiFile() const = 0;
};
