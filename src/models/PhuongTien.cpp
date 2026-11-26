#include "PhuongTien.h"
#include <iomanip>
#include <cmath>

using namespace std;

// =========================================================================
// 1. LỚP PHUONGTIEN
// =========================================================================
PhuongTien::PhuongTien()
    : bienSo(""), hangSanXuat(""), loaiPhuongTien("Chua xac dinh") {}

PhuongTien::PhuongTien(const string& bienSo, const string& hangSX, const string& loaiPT)
    : bienSo(bienSo), hangSanXuat(hangSX), loaiPhuongTien(loaiPT) {}

const string& PhuongTien::getBienSo() const {
    return bienSo;
}

const string& PhuongTien::getHangSanXuat() const {
    return hangSanXuat;
}

const string& PhuongTien::getLoaiPhuongTien() const {
    return loaiPhuongTien;
}

void PhuongTien::setBienSo(const string& bs) {
    bienSo = bs;
}

void PhuongTien::setHangSanXuat(const string& hang) {
    hangSanXuat = hang;
}

void PhuongTien::setLoaiPhuongTien(const string& loai) {
    loaiPhuongTien = loai;
}

void PhuongTien::hienThiThongTin() const {
    cout << "[" << loaiPhuongTien << "] Bien so: " << bienSo
         << " | Hang SX: " << hangSanXuat << "\n";
}

// =========================================================================
// 2. LỚP XETAI
// =========================================================================
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
         << " | Cuoc phi: 15,000 VND/km\n";
}

void XeTai::load() {
    cout << "[XeTai] Dang boc hang len thung xe " << bienSo << "...\n";
}

void XeTai::unload() {
    cout << "[XeTai] Dang do hang khoi thung xe " << bienSo << "...\n";
}

// =========================================================================
// 3. LỚP XETAILANH
// =========================================================================
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
         << " | Cuoc phi: 25,000 VND/km (Bao gom phi giu lanh)\n";
}

// =========================================================================
// 4. LỚP XEMAY
// =========================================================================
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

