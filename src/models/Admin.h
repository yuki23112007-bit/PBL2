#ifndef ADMIN_H
#define ADMIN_H

#include "TaiKhoan.h"

class Admin : public TaiKhoan {
public:
    Admin()
        : TaiKhoan("", "", "", "ADMIN") {}

    Admin(const string& id, const string& username, const string& passwordHash)
        : TaiKhoan(id, username, passwordHash, "ADMIN") {}

    ~Admin() override = default;

    void show() const override {
        cout << "[ADMIN] ID: " << id
             << " | Username: " << username << "\n";
    }

    string xuatChuoiFile() const override {
        return "ADMIN|" + id + "|" + username + "|" + passwordHash;
    }
};

#endif 

