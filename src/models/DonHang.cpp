#include "DonHang.h"

using namespace std;

DonHang::DonHang()
    : maDonHang(""), tenKhachHang(""), diaChiKhachHang(""), soDienThoaiKhachHang(""),
      trangThaiDonHang("Cho Duyet"), ngayDatHang(""), maNguoiDung(""), thoiGianTao(0) {}

DonHang::DonHang(const string& maDH, const string& tenKH,
                 const string& diaChi, const string& sdt,
                 const string& trangThai, const string& ngayDat,
                 const string& maND, time_t thoiGian)
    : maDonHang(maDH), tenKhachHang(tenKH), diaChiKhachHang(diaChi),
      soDienThoaiKhachHang(sdt), trangThaiDonHang(trangThai),
      ngayDatHang(ngayDat), maNguoiDung(maND),
      thoiGianTao(thoiGian == 0 ? time(nullptr) : thoiGian) {}

const string& DonHang::getMaDonHang() const {
    return maDonHang;
}

const string& DonHang::getTenKhachHang() const {
    return tenKhachHang;
}

const string& DonHang::getDiaChiKhachHang() const {
    return diaChiKhachHang;
}

const string& DonHang::getSoDienThoaiKhachHang() const {
    return soDienThoaiKhachHang;
}

const string& DonHang::getTrangThaiDonHang() const {
    return trangThaiDonHang;
}

const string& DonHang::getNgayDatHang() const {
    return ngayDatHang;
}

const string& DonHang::getMaNguoiDung() const {
    return maNguoiDung;
}

time_t DonHang::getThoiGianTao() const {
    return thoiGianTao;
}

void DonHang::setMaDonHang(const string& value) {
    maDonHang = value;
}

void DonHang::setTenKhachHang(const string& value) {
    tenKhachHang = value;
}

void DonHang::setDiaChiKhachHang(const string& value) {
    diaChiKhachHang = value;
}

void DonHang::setSoDienThoaiKhachHang(const string& value) {
    soDienThoaiKhachHang = value;
}

void DonHang::setTrangThaiDonHang(const string& value) {
    trangThaiDonHang = value;
}

void DonHang::setNgayDatHang(const string& value) {
    ngayDatHang = value;
}

void DonHang::setMaNguoiDung(const string& value) {
    maNguoiDung = value;
}

void DonHang::setThoiGianTao(time_t value) {
    thoiGianTao = value;
}

bool DonHang::daQuaHan5Phut(time_t thoiGianHienTai) const {
    return (thoiGianHienTai - thoiGianTao) >= 300;
}

bool DonHang::daQuaHan2Phut(time_t thoiGianHienTai) const {
    return (thoiGianHienTai - thoiGianTao) >= 120;
}

void DonHang::hienThiThongTin() const {
    cout << "Ma don: " << maDonHang
         << "\nKhach hang: " << tenKhachHang
         << "\nDia chi: " << diaChiKhachHang
         << "\nSDT: " << soDienThoaiKhachHang
         << "\nTrang thai: " << trangThaiDonHang
         << "\nNgay dat: " << ngayDatHang
         << "\nMa nguoi dung: " << maNguoiDung
         << "\nThoi gian tao: " << thoiGianTao << "\n";
}

string DonHang::xuatChuoiFile() const {
    return maDonHang + "|" + tenKhachHang + "|" + diaChiKhachHang + "|" +
           soDienThoaiKhachHang + "|" + trangThaiDonHang + "|" +
           ngayDatHang + "|" + maNguoiDung + "|" + to_string(static_cast<long long>(thoiGianTao));
}

ostream& operator<<(ostream& output, const DonHang& dh) {
    output << dh.xuatChuoiFile();
    return output;
}

istream& operator>>(istream& input, DonHang& dh) {
    string maDH, tenKH, diaChi, sdt, trangThai, ngayDat, maND;
    long long tg;
    if (input >> maDH >> tenKH >> diaChi >> sdt >> trangThai >> ngayDat >> maND >> tg) {
        dh.setMaDonHang(maDH);
        dh.setTenKhachHang(tenKH);
        dh.setDiaChiKhachHang(diaChi);
        dh.setSoDienThoaiKhachHang(sdt);
        dh.setTrangThaiDonHang(trangThai);
        dh.setNgayDatHang(ngayDat);
        dh.setMaNguoiDung(maND);
        dh.setThoiGianTao(static_cast<time_t>(tg));
    }
    return input;
}
