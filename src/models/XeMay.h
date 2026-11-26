#ifndef XEMAY_H
#define XEMAY_H

#include "PhuongTien.h"

// =========================================================================
// LỚP XE MÁY GIAO NHANH (Kế thừa PhuongTien)
// =========================================================================
class XeMay : public PhuongTien {
private:
    int dungTichXiLanh; // Dung tích xi lanh (cc, ví dụ: 125cc)

public:
    XeMay();
    XeMay(const string& bienSo, const string& hangSX, int dungTich = 125);
    ~XeMay() override = default;

    int getDungTichXiLanh() const;
    void setDungTichXiLanh(int dungTich);

    double tinhChiPhiVanChuyen(double quangDuongKm = 1.0) const override;
    void hienThiThongTin() const override;
};

#endif // XEMAY_H
