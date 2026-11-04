#include "DanhGiaDonHang.h"

using namespace std;

DanhGiaDonHang::DanhGiaDonHang()
    : maDanhGia(""), maDonHang(""), maNguoiDung(""), soSao(5),
      noiDung(""), thoiGianDanhGia(time(nullptr)), laKhieuNai(false) {}

DanhGiaDonHang::DanhGiaDonHang(const string& maDG, const string& maDH, const string& maND,
                               int sao, const string& noiDung, bool khieuNai, time_t thoiGian)
    : maDanhGia(maDG), maDonHang(maDH), maNguoiDung(maND), soSao(sao),
      noiDung(noiDung), thoiGianDanhGia(thoiGian == 0 ? time(nullptr) : thoiGian),
      laKhieuNai(khieuNai) {}

const string& DanhGiaDonHang::getMaDanhGia() const {
    return maDanhGia;
}

const string& DanhGiaDonHang::getMaDonHang() const {
    return maDonHang;
}

const string& DanhGiaDonHang::getMaNguoiDung() const {
    return maNguoiDung;
}

int DanhGiaDonHang::getSoSao() const {
    return soSao;
}

const string& DanhGiaDonHang::getNoiDung() const {
    return noiDung;
}

time_t DanhGiaDonHang::getThoiGianDanhGia() const {
    return thoiGianDanhGia;
}

bool DanhGiaDonHang::getLaKhieuNai() const {
    return laKhieuNai;
}

void DanhGiaDonHang::setMaDanhGia(const string& ma) {
    maDanhGia = ma;
}

void DanhGiaDonHang::setMaDonHang(const string& maDH) {
    maDonHang = maDH;
}

void DanhGiaDonHang::setMaNguoiDung(const string& maND) {
    maNguoiDung = maND;
}

void DanhGiaDonHang::setSoSao(int sao) {
    soSao = sao;
}

void DanhGiaDonHang::setNoiDung(const string& nd) {
    noiDung = nd;
}

void DanhGiaDonHang::setThoiGianDanhGia(time_t tg) {
    thoiGianDanhGia = tg;
}

void DanhGiaDonHang::setLaKhieuNai(bool khieuNai) {
    laKhieuNai = khieuNai;
}

bool DanhGiaDonHang::isKhieuNai() const {
    return laKhieuNai;
}

bool DanhGiaDonHang::isHaiLong() const {
    return soSao >= 4 && !laKhieuNai;
}

bool DanhGiaDonHang::coYKien() const {
    return !noiDung.empty();
}

void DanhGiaDonHang::hienThiThongTin() const {
    cout << "[DANH GIA / Y KIEN: " << maDanhGia << "]\n";
    cout << "  - Ma don hang : " << maDonHang << "\n";
    cout << "  - Khach hang  : " << maNguoiDung << "\n";
    cout << "  - Xep hang    : " << soSao << " sao\n";
    cout << "  - Phan loai   : " << (laKhieuNai ? "KHIEU NAI / SU CO" : "DANH GIA THUONG") << "\n";
    cout << "  - Y kien      : " << (noiDung.empty() ? "(Khong co y kien)" : noiDung) << "\n";
    cout << "  - Thoi gian   : " << thoiGianDanhGia << "\n";
}

string DanhGiaDonHang::xuatChuoiFile() const {
    return maDanhGia + "|" + maDonHang + "|" + maNguoiDung + "|" +
           to_string(soSao) + "|" + noiDung + "|" +
           to_string(static_cast<long long>(thoiGianDanhGia)) + "|" +
           (laKhieuNai ? "1" : "0");
}

ostream& operator<<(ostream& os, const DanhGiaDonHang& dg) {
    os << dg.xuatChuoiFile();
    return os;
}

istream& operator>>(istream& is, DanhGiaDonHang& dg) {
    string maDG, maDH, maND, nd, khieuNaiStr;
    int sao;
    long long tg;
    if (is >> maDG >> maDH >> maND >> sao >> nd >> tg >> khieuNaiStr) {
        dg.setMaDanhGia(maDG);
        dg.setMaDonHang(maDH);
        dg.setMaNguoiDung(maND);
        dg.setSoSao(sao);
        dg.setNoiDung(nd);
        dg.setThoiGianDanhGia(static_cast<time_t>(tg));
        dg.setLaKhieuNai(khieuNaiStr == "1" || khieuNaiStr == "true");
    }
    return is;
}

