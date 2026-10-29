#include "KhoHang.h"

KhoHang::KhoHang()
    : maKho(""), viTri(""), sucChuaToiDa(0), maSanPham("") {}

KhoHang::KhoHang(const string& maKho, const string& viTri,
                 int sucChua, const string& maSP)
    : maKho(maKho), viTri(viTri), sucChuaToiDa(sucChua), maSanPham(maSP) {}

const string& KhoHang::getMaKho() const {
    return maKho;
}

const string& KhoHang::getViTri() const {
    return viTri;
}

int KhoHang::getSucChuaToiDa() const {
    return sucChuaToiDa;
}

const string& KhoHang::getMaSanPham() const {
    return maSanPham;
}

void KhoHang::setMaKho(const string& value) {
    maKho = value;
}

void KhoHang::setViTri(const string& value) {
    viTri = value;
}

void KhoHang::setSucChuaToiDa(int value) {
    if (value >= 0) {
        sucChuaToiDa = value;
    }
}

void KhoHang::setMaSanPham(const string& value) {
    maSanPham = value;
}

void KhoHang::hienThiThongTin() const {
    cout << "Ma kho: " << maKho
         << "\nVi tri: " << viTri
         << "\nSuc chua toi da: " << sucChuaToiDa
         << "\nMa san pham: " << maSanPham << "\n";
}

string KhoHang::xuatChuoiFile() const {
    return maKho + "|" + viTri + "|" + to_string(sucChuaToiDa) + "|" + maSanPham;
}

ostream& operator<<(ostream& output, const KhoHang& kho) {
    output << kho.xuatChuoiFile();
    return output;
}

istream& operator>>(istream& input, KhoHang& kho) {
    string maKho, viTri, maSP;
    int sucChua;
    if (input >> maKho >> viTri >> sucChua >> maSP) {
        kho.setMaKho(maKho);
        kho.setViTri(viTri);
        kho.setSucChuaToiDa(sucChua);
        kho.setMaSanPham(maSP);
    }
    return input;
}
