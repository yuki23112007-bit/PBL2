#include "XeTai.h"
#include <iomanip>
#include <cmath>

using namespace std;

XeTai::XeTai()
    : PhuongTien("", "", "Xe Tai"), trongTaiToiDa(1.5) {}

XeTai::XeTai(const string& bienSo, const string& hangSX, double trongTai)
    : PhuongTien(bienSo, hangSX, "Xe Tai"), trongTaiToiDa(trongTai) {}

double XeTai::getTrongTaiToiDa() const {
    return trongTaiToiDa;
}

void XeTai::setTrongTaiToiDa(double trongTai) {
    trongTaiToiDa = trongTai;
}

double XeTai::tinhChiPhiVanChuyen(double quangDuongKm) const {
    // Nội thành (<= 15km): 15.000đ/km; Liên tỉnh (> 15km, ghép hàng, không phí mở cửa): 250đ/km (tối thiểu 40.000đ)
    if (quangDuongKm <= 0) quangDuongKm = 1.0;
    if (quangDuongKm <= 15.0) {
        return round((quangDuongKm * 15000.0) / 1000.0) * 1000.0;
    }
    double phi = round((quangDuongKm * 250.0) / 1000.0) * 1000.0;
    return phi < 40000.0 ? 40000.0 : phi;
}

void XeTai::hienThiThongTin() const {
    cout << "[XE TAI] Bien so: " << bienSo
         << " | Hang: " << hangSanXuat
         << " | Trong tai: " << fixed << setprecision(1) << trongTaiToiDa << " tan"
         << " | Cuoc phi: 15,000 VND/km (noi thanh) / 250 VND/km (lien tinh)\n";
}

void XeTai::load() {
    cout << "[XeTai] Dang boc hang len thung xe " << bienSo << "...\n";
}

void XeTai::unload() {
    cout << "[XeTai] Dang do hang khoi thung xe " << bienSo << "...\n";
}
