#include "XeMay.h"
#include <cmath>

using namespace std;

XeMay::XeMay()
    : PhuongTien("", "", "Xe May Giao Nhanh"), dungTichXiLanh(125) {}

XeMay::XeMay(const string& bienSo, const string& hangSX, int dungTich)
    : PhuongTien(bienSo, hangSX, "Xe May Giao Nhanh"), dungTichXiLanh(dungTich) {}

int XeMay::getDungTichXiLanh() const {
    return dungTichXiLanh;
}

void XeMay::setDungTichXiLanh(int dungTich) {
    dungTichXiLanh = dungTich;
}

double XeMay::tinhChiPhiVanChuyen(double quangDuongKm) const {
    // Nội thành (<= 15km): <= 2.5km cố định 25.000đ, xa hơn 10.000đ/km; Liên tỉnh (> 15km, không phí mở cửa): 200đ/km (tối thiểu 30.000đ)
    if (quangDuongKm <= 0) quangDuongKm = 1.0;
    if (quangDuongKm <= 2.5) {
        return 25000.0;
    }
    if (quangDuongKm <= 15.0) {
        return round((quangDuongKm * 10000.0) / 1000.0) * 1000.0;
    }
    double phi = round((quangDuongKm * 200.0) / 1000.0) * 1000.0;
    return phi < 30000.0 ? 30000.0 : phi;
}

void XeMay::hienThiThongTin() const {
    cout << "[XE MAY GIAO NHANH] Bien so: " << bienSo
         << " | Hang: " << hangSanXuat
         << " | Phan khoi: " << dungTichXiLanh << " cc"
         << " | Phi ship co ban: 25,000 VND\n";
}
