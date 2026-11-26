#include "XeTaiLanh.h"
#include <iomanip>
#include <cmath>

using namespace std;

XeTaiLanh::XeTaiLanh()
    : XeTai("", "", 2.0), nhietDoThungXe(-18.0) {
    loaiPhuongTien = "Xe Tai Lanh Chuyen Dung";
}

XeTaiLanh::XeTaiLanh(const string& bienSo, const string& hangSX, double trongTai, double nhietDo)
    : XeTai(bienSo, hangSX, trongTai), nhietDoThungXe(nhietDo) {
    loaiPhuongTien = "Xe Tai Lanh Chuyen Dung";
}

double XeTaiLanh::getNhietDoThungXe() const {
    return nhietDoThungXe;
}

void XeTaiLanh::setNhietDoThungXe(double nhietDo) {
    nhietDoThungXe = nhietDo;
}

double XeTaiLanh::tinhChiPhiVanChuyen(double quangDuongKm) const {
    // Nội thành (<= 15km): 25.000đ/km; Liên tỉnh (> 15km, ghép hàng lạnh, không phí mở cửa): 350đ/km (tối thiểu 55.000đ)
    if (quangDuongKm <= 0) quangDuongKm = 1.0;
    if (quangDuongKm <= 15.0) {
        return round((quangDuongKm * 25000.0) / 1000.0) * 1000.0;
    }
    double phi = round((quangDuongKm * 350.0) / 1000.0) * 1000.0;
    return phi < 55000.0 ? 55000.0 : phi;
}

void XeTaiLanh::hienThiThongTin() const {
    cout << "[XE TAI LANH] Bien so: " << bienSo
         << " | Hang: " << hangSanXuat
         << " | Trong tai: " << fixed << setprecision(1) << trongTaiToiDa << " tan"
         << " | Nhiet do bao quan: " << fixed << setprecision(1) << nhietDoThungXe << " do C"
         << " | Cuoc phi: 25,000 VND/km (noi thanh) / 350 VND/km (lien tinh)\n";
}
