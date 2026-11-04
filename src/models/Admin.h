#ifndef ADMIN_H
#define ADMIN_H

#include "TaiKhoan.h"
#include <string>

using namespace std;

class Admin : public TaiKhoan {
public:
    Admin();
    Admin(const string& id, const string& username, const string& passwordHash);
    ~Admin() override = default;

    // Getter & Setter theo tài liệu
    const string& getAdminId() const { return getId(); }
    void setAdminId(const string& v) { setId(v); }

    void show() const override;
    void hienThiThongTin() const override { show(); }
    string xuatChuoiFile() const override;

    // Đa năng hóa toán tử xuất luồng
    friend ostream& operator<<(ostream& os, const Admin& ad) {
        os << ad.xuatChuoiFile();
        return os;
    }
};

#endif // ADMIN_H
