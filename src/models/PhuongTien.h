#ifndef PHUONGTIEN_H
#define PHUONGTIEN_H

#include <iostream>
#include <string>

using namespace std;

// =========================================================================
// TẦNG 1: LỚP CƠ SỞ TRỪU TƯỢNG PHƯƠNG TIỆN (Abstract Base Class)
// =========================================================================
class PhuongTien {
protected:
    string bienSo;         // Biển số xe (ví dụ: "43C-12345")
    string hangSanXuat;    // Hãng sản xuất (ví dụ: "Hino", "Isuzu", "Honda")
    string loaiPhuongTien; // Tên phân loại phương tiện

public:
    PhuongTien();
    PhuongTien(const string& bienSo, const string& hangSX, const string& loaiPT);
    virtual ~PhuongTien() = default;

    // Getter
    const string& getBienSo() const;
    const string& getHangSanXuat() const;
    const string& getLoaiPhuongTien() const;

    // Setter
    void setBienSo(const string& bs);
    void setHangSanXuat(const string& hang);
    void setLoaiPhuongTien(const string& loai);

    // Tính đa hình (Polymorphism): Mỗi loại phương tiện tính giá cước vận chuyển khác nhau
    virtual double tinhChiPhiVanChuyen(double quangDuongKm = 1.0) const = 0;
    virtual void hienThiThongTin() const;
    void show() const { hienThiThongTin(); }
};

#endif // PHUONGTIEN_H
