#include "NhaCungCap.h"

using namespace std;

NhaCungCap::NhaCungCap()
    : maNhaCungCap(""), tenCongTy(""), diaChi(""), soDienThoai(""), danhMucCungCap("") {}

NhaCungCap::NhaCungCap(const string& maNCC, const string& tenCT,
                       const string& diaChi, const string& sdt,
                       const string& danhMuc)
    : maNhaCungCap(maNCC), tenCongTy(tenCT), diaChi(diaChi),
      soDienThoai(sdt), danhMucCungCap(danhMuc) {}

const string& NhaCungCap::getMaNhaCungCap() const {
    return maNhaCungCap;
}

const string& NhaCungCap::getTenCongTy() const {
    return tenCongTy;
}

const string& NhaCungCap::getDiaChi() const {
    return diaChi;
}

const string& NhaCungCap::getSoDienThoai() const {
    return soDienThoai;
}

const string& NhaCungCap::getDanhMucCungCap() const {
    return danhMucCungCap;
}

void NhaCungCap::setMaNhaCungCap(const string& ma) {
    maNhaCungCap = ma;
}

void NhaCungCap::setTenCongTy(const string& ten) {
    tenCongTy = ten;
}

void NhaCungCap::setDiaChi(const string& dc) {
    diaChi = dc;
}

void NhaCungCap::setSoDienThoai(const string& sdt) {
    soDienThoai = sdt;
}

void NhaCungCap::setDanhMucCungCap(const string& dm) {
    danhMucCungCap = dm;
}

void NhaCungCap::hienThiThongTin() const {
    cout << "[NHA CUNG CAP " << maNhaCungCap << "]\n";
    cout << "  - Ten cong ty : " << tenCongTy << "\n";
    cout << "  - Dia chi     : " << diaChi << "\n";
    cout << "  - Dien thoai  : " << soDienThoai << "\n";
    cout << "  - Danh muc    : " << danhMucCungCap << "\n";
}

string NhaCungCap::xuatChuoiFile() const {
    return maNhaCungCap + "|" + tenCongTy + "|" + diaChi + "|" +
           soDienThoai + "|" + danhMucCungCap;
}

ostream& operator<<(ostream& os, const NhaCungCap& ncc) {
    os << ncc.xuatChuoiFile();
    return os;
}

istream& operator>>(istream& is, NhaCungCap& ncc) {
    is >> ncc.maNhaCungCap >> ncc.tenCongTy >> ncc.diaChi
       >> ncc.soDienThoai >> ncc.danhMucCungCap;
    return is;
}
