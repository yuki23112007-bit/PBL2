#include "ThucPhamThuong.h"

using namespace std;

ThucPhamThuong::ThucPhamThuong()
    : ThucPham(), quyCachDongGoi("") {
    LoaiSP = "THUONG";
}

ThucPhamThuong::ThucPhamThuong(const ThucPhamThuong& other)
    : ThucPham(other), quyCachDongGoi(other.quyCachDongGoi) {
    LoaiSP = "THUONG";
}

ThucPhamThuong::ThucPhamThuong(const string& maSP, const string& tenSP,
                             const string& danhMuc, const double& donGia,
                             const int& soLuongTonKho, const string& maNCC,
                             const time_t& ngaySX, const int& hanDungSoNgay,
                             const string& quyCach)
    : ThucPham(maSP, tenSP, danhMuc, donGia, soLuongTonKho, "THUONG", maNCC, ngaySX, hanDungSoNgay),
      quyCachDongGoi(quyCach) {}

const string& ThucPhamThuong::getQuyCachDongGoi() const {
    return quyCachDongGoi;
}

void ThucPhamThuong::setQuyCachDongGoi(const string& value) {
    quyCachDongGoi = value;
}

bool ThucPhamThuong::canbaoquanlanh() const {
    return false;
}

void ThucPhamThuong::inra(ostream& output) const {
    ThucPham::inra(output);
    output << "\nQuy cach dong goi: " << quyCachDongGoi;
}

void ThucPhamThuong::docvao(istream& input) {
    ThucPham::docvao(input);
    input >> quyCachDongGoi;
}

string ThucPhamThuong::xuatchuoifile() const {
    return "THUONG|" + ChuoiPhanChung() + "|" + quyCachDongGoi + "|" + HanSuDung();
}

