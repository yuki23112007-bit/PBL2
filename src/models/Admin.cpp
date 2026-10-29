#include "Admin.h"

using namespace std;

Admin::Admin()
    : TaiKhoan("", "", "", "ADMIN") {}

Admin::Admin(const string& id, const string& username, const string& passwordHash)
    : TaiKhoan(id, username, passwordHash, "ADMIN") {}

void Admin::show() const {
    cout << "[ADMIN] ID: " << id
         << " | Username: " << username << "\n";
}

string Admin::xuatChuoiFile() const {
    return "ADMIN|" + id + "|" + username + "|" + passwordHash;
}

