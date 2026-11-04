#ifndef NGUOIDUNG_H
#define NGUOIDUNG_H

#include "TaiKhoan.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Khai báo chuyển tiếp (Forward Declaration)
class DonHang;

class NguoiDung : public TaiKhoan {
private:
    string hoTen;
    string soDienThoai;
    string diaChi;
    vector<DonHang*> danhSachDonHangCaNhan; // Chứa con trỏ tới các đơn hàng của khách (O(k) tra cứu)

public:
    NguoiDung();
    NguoiDung(const string& id, const string& username, const string& passwordHash,
              const string& hoTen, const string& soDienThoai, const string& diaChi);
    ~NguoiDung() override = default;

    // Getter
    const string& getHoTen() const;
    const string& getSoDienThoai() const;
    const string& getDiaChi() const;
    const vector<DonHang*>& getDanhSachDonHang() const;

    // Bí danh theo tài liệu
    const string& getMaNguoiDung() const { return getId(); }
    void setMaNguoiDung(const string& v) { setId(v); }

    // Setter
    void setHoTen(const string& v);
    void setSoDienThoai(const string& v);
    void setDiaChi(const string& v);

    // Nghiệp vụ theo mục 5.1 README
    void themDonHang(DonHang* dh);

    void show() const override;
    void hienThiThongTin() const override { show(); }
    string xuatChuoiFile() const override;

    // Đa năng hóa toán tử xuất luồng
    friend ostream& operator<<(ostream& os, const NguoiDung& nd) {
        os << nd.xuatChuoiFile();
        return os;
    }
};

#endif // NGUOIDUNG_H
