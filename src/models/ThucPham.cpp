#include "ThucPham.h"

using namespace std;

ThucPham::ThucPham()
    : SanPham(), HanDungSoNgay(0) {}

ThucPham::ThucPham(const ThucPham& other)
    : SanPham(other), HanDungSoNgay(other.HanDungSoNgay) {}

ThucPham::ThucPham(const string& maSP, const string& tenSP,
                   const string& danhMuc, const double& donGia,
                   const int& soLuongTonKho, const string& loaiSP,
                   const string& maNCC, const time_t& ngaySX,
                   const int& hanDungSoNgay)
    : SanPham(maSP, tenSP, danhMuc, donGia, soLuongTonKho, loaiSP, maNCC, ngaySX),
      HanDungSoNgay(hanDungSoNgay) {}

ThucPham::~ThucPham() = default;

int ThucPham::getHanDungSoNgay() const {
    return HanDungSoNgay;
}

void ThucPham::setHanDungSoNgay(const int& value) {
    HanDungSoNgay = value;
}

time_t ThucPham::ngayhethang() const {
    constexpr time_t secondsPerDay = 24 * 60 * 60;
    return NgaySX + static_cast<time_t>(HanDungSoNgay) * secondsPerDay;
}

bool ThucPham::daHetHan(time_t thoiDiemHienTai) const {
    if (thoiDiemHienTai == 0) {
        thoiDiemHienTai = time(nullptr);
    }
    return thoiDiemHienTai > ngayhethang();
}

double ThucPham::soNgayConLai(time_t thoiDiemHienTai) const {
    if (thoiDiemHienTai == 0) {
        thoiDiemHienTai = time(nullptr);
    }
    return difftime(ngayhethang(), thoiDiemHienTai) / 86400.0;
}

string ThucPham::HanSuDung() const {
    return to_string(static_cast<long long>(NgaySX)) + "|" +
           to_string(HanDungSoNgay);
}

void ThucPham::inra(ostream& output) const {
    SanPham::inra(output);
    output << "\nHan dung (ngay): " << HanDungSoNgay;
}

void ThucPham::docvao(istream& input) {
    SanPham::docvao(input);
    input >> HanDungSoNgay;
}

