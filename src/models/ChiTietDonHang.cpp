#include "ChiTietDonHang.h"

#include <iomanip>
#include <limits>
#include <sstream>

using namespace std;

static string SoThanhChuoi(double val) {
    ostringstream oss;
    oss << setprecision(numeric_limits<double>::max_digits10) << val;
    return oss.str();
}

ChiTietDonHang::ChiTietDonHang()
    : maChiTiet(""), maDonHang(""), maSanPham(""), soLuong(0), thanhTien(0.0) {}

ChiTietDonHang::ChiTietDonHang(const string& maCT, const string& maDH,
                               const string& maSP, int soLuong, double donGia)
    : maChiTiet(maCT), maDonHang(maDH), maSanPham(maSP),
      soLuong(soLuong), thanhTien(soLuong * donGia) {}

const string& ChiTietDonHang::getMaChiTiet() const {
    return maChiTiet;
}

const string& ChiTietDonHang::getMaDonHang() const {
    return maDonHang;
}

const string& ChiTietDonHang::getMaSanPham() const {
    return maSanPham;
}

int ChiTietDonHang::getSoLuong() const {
    return soLuong;
}

double ChiTietDonHang::getThanhTien() const {
    return thanhTien;
}

void ChiTietDonHang::setMaChiTiet(const string& value) {
    maChiTiet = value;
}

void ChiTietDonHang::setMaDonHang(const string& value) {
    maDonHang = value;
}

void ChiTietDonHang::setMaSanPham(const string& value) {
    maSanPham = value;
}

void ChiTietDonHang::setSoLuong(int value) {
    if (value >= 0) {
        soLuong = value;
    }
}

void ChiTietDonHang::setThanhTien(double value) {
    if (value >= 0.0) {
        thanhTien = value;
    }
}

void ChiTietDonHang::tinhThanhTien(double donGia) {
    thanhTien = soLuong * donGia;
}

void ChiTietDonHang::hienThiThongTin() const {
    cout << "Ma CT: " << maChiTiet
         << " | Ma DH: " << maDonHang
         << " | Ma SP: " << maSanPham
         << " | So luong: " << soLuong
         << " | Thanh tien: " << thanhTien << "\n";
}

string ChiTietDonHang::xuatChuoiFile() const {
    return maChiTiet + "|" + maDonHang + "|" + maSanPham + "|" +
           to_string(soLuong) + "|" + SoThanhChuoi(thanhTien);
}

ostream& operator<<(ostream& output, const ChiTietDonHang& ct) {
    output << ct.xuatChuoiFile();
    return output;
}

istream& operator>>(istream& input, ChiTietDonHang& ct) {
    string maCT, maDH, maSP;
    int sl;
    double tt;
    if (input >> maCT >> maDH >> maSP >> sl >> tt) {
        ct.setMaChiTiet(maCT);
        ct.setMaDonHang(maDH);
        ct.setMaSanPham(maSP);
        ct.setSoLuong(sl);
        ct.setThanhTien(tt);
    }
    return input;
}
