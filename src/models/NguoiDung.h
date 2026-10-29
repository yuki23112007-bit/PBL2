#ifndef NGUOIDUNG_H
#define NGUOIDUNG_H

#include "TaiKhoan.h"
#include "DonHang.h"
#include <vector>

class NguoiDung : public TaiKhoan {
private:
    string hoTen;
    string soDienThoai;
    string diaChi;
    vector<DonHang*> danhSachDonHangCaNhan; // Chứa con trỏ tới các đơn hàng của khách (O(k) tra cứu)

public:
    NguoiDung()
        : TaiKhoan("", "", "", "CUSTOMER"), hoTen(""), soDienThoai(""), diaChi("") {}

    NguoiDung(const string& id, const string& username, const string& passwordHash,
              const string& hoTen, const string& soDienThoai, const string& diaChi)
        : TaiKhoan(id, username, passwordHash, "CUSTOMER"),
          hoTen(hoTen), soDienThoai(soDienThoai), diaChi(diaChi) {}

    // Theo mục 5.1 README: NguoiDung chỉ giữ con trỏ mượn từ QuanLyDonHang, không giải phóng bộ nhớ
    ~NguoiDung() override = default;

    // Getter
    const string& getHoTen() const { return hoTen; }
    const string& getSoDienThoai() const { return soDienThoai; }
    const string& getDiaChi() const { return diaChi; }
    const vector<DonHang*>& getDanhSachDonHang() const { return danhSachDonHangCaNhan; }

    // Setter
    void setHoTen(const string& v) { hoTen = v; }
    void setSoDienThoai(const string& v) { soDienThoai = v; }
    void setDiaChi(const string& v) { diaChi = v; }

    // Nghiệp vụ theo mục 5.1 README
    void themDonHang(DonHang* dh) {
        if (dh != nullptr) {
            danhSachDonHangCaNhan.push_back(dh);
        }
    }

    void show() const override {
        cout << "[CUSTOMER] ID: " << id
             << " | Username: " << username
             << " | Ho ten: " << hoTen
             << " | SDT: " << soDienThoai
             << " | Dia chi: " << diaChi
             << " | So don hang: " << danhSachDonHangCaNhan.size() << "\n";
    }

    string xuatChuoiFile() const override {
        return "CUSTOMER|" + id + "|" + username + "|" + passwordHash + "|" +
               hoTen + "|" + soDienThoai + "|" + diaChi;
    }
};

#endif

