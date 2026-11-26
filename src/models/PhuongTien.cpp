#include "PhuongTien.h"

using namespace std;

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
