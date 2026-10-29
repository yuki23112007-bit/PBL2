#include "ThucPhamTuoiSong.h"

using namespace std;

ThucPhamTuoiSong::ThucPhamTuoiSong()
    : ThucPham(), nhietDoBaoQuan(0.0) {
    LoaiSP = "TUOI_SONG";
}

ThucPhamTuoiSong::ThucPhamTuoiSong(const ThucPhamTuoiSong& other)
    : ThucPham(other), nhietDoBaoQuan(other.nhietDoBaoQuan) {
    LoaiSP = "TUOI_SONG";
}

ThucPhamTuoiSong::ThucPhamTuoiSong(const string& maSP, const string& tenSP,
                                   const string& danhMuc, const double& donGia,
                                   const int& soLuongTonKho, const string& maNCC,
                                   const time_t& ngaySX, const int& hanDungSoNgay,
                                   const double& nhietDo)
    : ThucPham(maSP, tenSP, danhMuc, donGia, soLuongTonKho, "TUOI_SONG", maNCC, ngaySX, hanDungSoNgay),
      nhietDoBaoQuan(nhietDo) {}

double ThucPhamTuoiSong::getNhietDoBaoQuan() const {
    return nhietDoBaoQuan;
}

void ThucPhamTuoiSong::setNhietDoBaoQuan(const double& value) {
    nhietDoBaoQuan = value;
}

bool ThucPhamTuoiSong::canbaoquanlanh() const {
    return true;
}

void ThucPhamTuoiSong::inra(ostream& output) const {
    ThucPham::inra(output);
    output << "\nNhiet do bao quan: " << nhietDoBaoQuan << " do C";
}

void ThucPhamTuoiSong::docvao(istream& input) {
    ThucPham::docvao(input);
    input >> nhietDoBaoQuan;
}

string ThucPhamTuoiSong::xuatchuoifile() const {
    return "TUOI_SONG|" + ChuoiPhanChung() + "|" + SoThanhChuoi(nhietDoBaoQuan) + "|" + HanSuDung();
}

