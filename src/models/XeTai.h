#ifndef XETAI_H
#define XETAI_H

#include "PhuongTien.h"

// =========================================================================
// TẦNG 2: LỚP TRUNG GIAN XE TẢI (Kế thừa PhuongTien)
// =========================================================================
class XeTai : public PhuongTien {
protected:
    double trongTaiToiDa; // Tải trọng tối đa tính bằng tấn (ví dụ: 2.5 tấn)

public:
    XeTai();
    XeTai(const string& bienSo, const string& hangSX, double trongTai);
    ~XeTai() override = default;

    double getTrongTaiToiDa() const;
    void setTrongTaiToiDa(double trongTai);

    double tinhChiPhiVanChuyen(double quangDuongKm = 1.0) const override;
    void hienThiThongTin() const override;

    virtual void load();
    virtual void unload();
};

#endif // XETAI_H
