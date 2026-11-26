#ifndef XETAILANH_H
#define XETAILANH_H

#include "XeTai.h"

// =========================================================================
// TẦNG 3: LỚP CON CHUYÊN BIỆT XE TẢI LẠNH (Kế thừa nhiều tầng từ XeTai)
// =========================================================================
class XeTaiLanh : public XeTai {
private:
    double nhietDoThungXe; // Nhiệt độ thùng đông lạnh (°C, ví dụ: -18.0°C để chở đồ tươi sống)

public:
    XeTaiLanh();
    XeTaiLanh(const string& bienSo, const string& hangSX, double trongTai, double nhietDo);
    ~XeTaiLanh() override = default;

    double getNhietDoThungXe() const;
    void setNhietDoThungXe(double nhietDo);

    double tinhChiPhiVanChuyen(double quangDuongKm = 1.0) const override;
    void hienThiThongTin() const override;
};

#endif // XETAILANH_H
