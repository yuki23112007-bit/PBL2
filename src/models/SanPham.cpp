#include "SanPham.h"

#include <iomanip>
#include <limits>
#include <sstream>

using namespace std;

SanPham::SanPham()
    : MaSP(), TenSP(), DandMuc(), DonGia(0.0), SoLuongTonKho(0),
      LoaiSP(), MaNCC(), NgaySX(0), HanDungSoNgay(0) {}

SanPham::SanPham(const SanPham& other)
    : MaSP(other.MaSP), TenSP(other.TenSP), DandMuc(other.DandMuc),
      DonGia(other.DonGia), SoLuongTonKho(other.SoLuongTonKho),
      LoaiSP(other.LoaiSP), MaNCC(other.MaNCC), NgaySX(other.NgaySX),
      HanDungSoNgay(other.HanDungSoNgay) {}

SanPham::SanPham(const string& maSP, const string& tenSP,
                 const string& danhMuc, const double& donGia,
                 const int& soLuongTonKho, const string& loaiSP,
                 const string& maNCC, const time_t& ngaySX,
                 const int& hanDungSoNgay)
    : MaSP(maSP), TenSP(tenSP), DandMuc(danhMuc), DonGia(donGia),
      SoLuongTonKho(soLuongTonKho), LoaiSP(loaiSP), MaNCC(maNCC),
      NgaySX(ngaySX), HanDungSoNgay(hanDungSoNgay) {}

SanPham::~SanPham() = default;

const string& SanPham::getMaSP() const { return MaSP; }
const string& SanPham::getTenSP() const { return TenSP; }
const string& SanPham::getDanhMuc() const { return DandMuc; }
double SanPham::getDonGia() const { return DonGia; }
int SanPham::getSoLuongTonKho() const { return SoLuongTonKho; }
const string SanPham::getLoaiSP() const { return LoaiSP; }
const string SanPham::getMaNCC() const { return MaNCC; }
time_t SanPham::getNgaySX() const { return NgaySX; }
int SanPham::getHanDungSoNgay() const { return HanDungSoNgay; }

void SanPham::setMaSP(const string& value) { MaSP = value; }
void SanPham::setTenSP(const string& value) { TenSP = value; }
void SanPham::setDanhMuc(const string& value) { DandMuc = value; }
void SanPham::setDonGia(const double& value) { DonGia = value; }
void SanPham::setSoLuongTonKho(const int& value) { SoLuongTonKho = value; }
void SanPham::setLoaiSP(const string& value) { LoaiSP = value; }
void SanPham::setMaNCC(const string& value) { MaNCC = value; }
void SanPham::setNgaySX(const time_t& value) { NgaySX = value; }
void SanPham::setHanDungSoNgay(const int& value) { HanDungSoNgay = value; }

bool SanPham::CapNhatSoLuongTonKho(const int& soLuongMoi) {
    if (soLuongMoi < 0) {
        return false;
    }

    SoLuongTonKho = soLuongMoi;
    return true;
}

bool SanPham::KiemTranCanhBaoHetHang(const int& nguong) const {
    return SoLuongTonKho <= nguong;
}

time_t SanPham::ngayhethang() const {
    constexpr time_t secondsPerDay = 24 * 60 * 60;
    return NgaySX + static_cast<time_t>(HanDungSoNgay) * secondsPerDay;
}

void SanPham::show() const {
    inra(cout);
}

bool SanPham::canbaoquanlanh() const {
    return false;
}

string SanPham::ChuoiPhanChung() const {
    return MaSP + "|" + TenSP + "|" + DandMuc + "|" +
           SoThanhChuoi(DonGia) + "|" + to_string(SoLuongTonKho) + "|" +
           MaNCC;
}

string SanPham::HanSuDung() const {
    return to_string(static_cast<long long>(NgaySX)) + "|" +
           to_string(HanDungSoNgay);
}

string SanPham::SoThanhChuoi(const double& value) {
    ostringstream stream;
    stream << setprecision(numeric_limits<double>::max_digits10) << value;
    return stream.str();
}

void SanPham::inra(ostream& output) const {
    output << "Ma san pham: " << MaSP
           << "\nTen san pham: " << TenSP
           << "\nDanh muc: " << DandMuc
           << "\nDon gia: " << DonGia
           << "\nSo luong ton kho: " << SoLuongTonKho
           << "\nLoai san pham: " << LoaiSP
           << "\nMa nha cung cap: " << MaNCC
           << "\nNgay san xuat (time_t): " << NgaySX
           << "\nHan dung (ngay): " << HanDungSoNgay;
}

void SanPham::docvao(istream& input) {
    string maSP, tenSP, danhMuc, loaiSP, maNCC;
    double donGia;
    int soLuongTonKho, hanDungSoNgay;
    time_t ngaySX;

    if (input >> maSP >> tenSP >> danhMuc >> donGia >> soLuongTonKho
              >> loaiSP >> maNCC >> ngaySX >> hanDungSoNgay) {
        MaSP = maSP;
        TenSP = tenSP;
        DandMuc = danhMuc;
        DonGia = donGia;
        SoLuongTonKho = soLuongTonKho;
        LoaiSP = loaiSP;
        MaNCC = maNCC;
        NgaySX = ngaySX;
        HanDungSoNgay = hanDungSoNgay;
    }
}

ostream& operator<<(ostream& output, const SanPham& sanPham) {
    sanPham.inra(output);
    return output;
}

istream& operator>>(istream& input, SanPham& sanPham) {
    sanPham.docvao(input);
    return input;
}
