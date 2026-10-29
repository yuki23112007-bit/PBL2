#include "SanPhamTuoiSong.h"

using namespace std;

SanPhamTuoiSong::SanPhamTuoiSong()
    : SanPham(), nhietDoBaoQuan(0.0) {
    LoaiSP = "TUOI_SONG";
}

SanPhamTuoiSong::SanPhamTuoiSong(const SanPhamTuoiSong& other)
    : SanPham(other), nhietDoBaoQuan(other.nhietDoBaoQuan) {
    LoaiSP = "TUOI_SONG";
}

SanPhamTuoiSong::SanPhamTuoiSong(const string& maSP, const string& tenSP,
                                 const string& danhMuc, const double& donGia,
                                 const int& soLuongTonKho, const string& maNCC,
                                 const time_t& ngaySX, const int& hanDungSoNgay,
                                 const double& nhietDo)
    : SanPham(maSP, tenSP, danhMuc, donGia, soLuongTonKho, "TUOI_SONG", maNCC, ngaySX, hanDungSoNgay),
      nhietDoBaoQuan(nhietDo) {}

double SanPhamTuoiSong::getNhietDoBaoQuan() const {
    return nhietDoBaoQuan;
}

void SanPhamTuoiSong::setNhietDoBaoQuan(const double& value) {
    nhietDoBaoQuan = value;
}

bool SanPhamTuoiSong::canbaoquanlanh() const {
    return true;
}

void SanPhamTuoiSong::inra(ostream& output) const {
    SanPham::inra(output);
    output << "\nNhiet do bao quan: " << nhietDoBaoQuan << " do C";
}

void SanPhamTuoiSong::docvao(istream& input) {
    SanPham::docvao(input);
    input >> nhietDoBaoQuan;
}

string SanPhamTuoiSong::xuatchuoifile() const {
    return "TUOI_SONG|" + ChuoiPhanChung() + "|" + SoThanhChuoi(nhietDoBaoQuan) + "|" + HanSuDung();
}
