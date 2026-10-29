#include "SanPhamThuong.h"

using namespace std;

SanPhamThuong::SanPhamThuong()
    : SanPham(), quyCachDongGoi("") {
    LoaiSP = "THUONG";
}

SanPhamThuong::SanPhamThuong(const SanPhamThuong& other)
    : SanPham(other), quyCachDongGoi(other.quyCachDongGoi) {
    LoaiSP = "THUONG";
}

SanPhamThuong::SanPhamThuong(const string& maSP, const string& tenSP,
                             const string& danhMuc, const double& donGia,
                             const int& soLuongTonKho, const string& maNCC,
                             const time_t& ngaySX, const int& hanDungSoNgay,
                             const string& quyCach)
    : SanPham(maSP, tenSP, danhMuc, donGia, soLuongTonKho, "THUONG", maNCC, ngaySX, hanDungSoNgay),
      quyCachDongGoi(quyCach) {}

const string& SanPhamThuong::getQuyCachDongGoi() const {
    return quyCachDongGoi;
}

void SanPhamThuong::setQuyCachDongGoi(const string& value) {
    quyCachDongGoi = value;
}

bool SanPhamThuong::canbaoquanlanh() const {
    return false;
}

void SanPhamThuong::inra(ostream& output) const {
    SanPham::inra(output);
    output << "\nQuy cach dong goi: " << quyCachDongGoi;
}

void SanPhamThuong::docvao(istream& input) {
    SanPham::docvao(input);
    input >> quyCachDongGoi;
}

string SanPhamThuong::xuatchuoifile() const {
    return "THUONG|" + ChuoiPhanChung() + "|" + quyCachDongGoi + "|" + HanSuDung();
}
