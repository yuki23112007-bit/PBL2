#include "NguoiDung.h"

using namespace std;

NguoiDung::NguoiDung()
    : TaiKhoan("", "", "", "CUSTOMER"), hoTen(""), soDienThoai(""), diaChi("") {}

NguoiDung::NguoiDung(const string& id, const string& username, const string& passwordHash,
                     const string& hoTen, const string& soDienThoai, const string& diaChi)
    : TaiKhoan(id, username, passwordHash, "CUSTOMER"),
      hoTen(hoTen), soDienThoai(soDienThoai), diaChi(diaChi) {}

const string& NguoiDung::getHoTen() const {
    return hoTen;
}

const string& NguoiDung::getSoDienThoai() const {
    return soDienThoai;
}

const string& NguoiDung::getDiaChi() const {
    return diaChi;
}

const vector<DonHang*>& NguoiDung::getDanhSachDonHang() const {
    return danhSachDonHangCaNhan;
}

void NguoiDung::setHoTen(const string& v) {
    hoTen = v;
}

void NguoiDung::setSoDienThoai(const string& v) {
    soDienThoai = v;
}

void NguoiDung::setDiaChi(const string& v) {
    diaChi = v;
}

void NguoiDung::themDonHang(DonHang* dh) {
    if (dh != nullptr) {
        danhSachDonHangCaNhan.push_back(dh);
    }
}

void NguoiDung::show() const {
    cout << "[CUSTOMER] ID: " << id
         << " | Username: " << username
         << " | Ho ten: " << hoTen
         << " | SDT: " << soDienThoai
         << " | Dia chi: " << diaChi
         << " | So don hang: " << danhSachDonHangCaNhan.size() << "\n";
}

string NguoiDung::xuatChuoiFile() const {
    return "CUSTOMER|" + id + "|" + username + "|" + passwordHash + "|" +
           hoTen + "|" + soDienThoai + "|" + diaChi;
}
