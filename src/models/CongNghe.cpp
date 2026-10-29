#include "CongNghe.h"

using namespace std;

CongNghe::CongNghe()
    : SanPham(), thoiGianBaoHanh(12), canBaoQuanCanThan(true) {
    LoaiSP = "CONG_NGHE";
}

CongNghe::CongNghe(const CongNghe& other)
    : SanPham(other), thoiGianBaoHanh(other.thoiGianBaoHanh),
      canBaoQuanCanThan(other.canBaoQuanCanThan) {
    LoaiSP = "CONG_NGHE";
}

CongNghe::CongNghe(const string& maSP, const string& tenSP,
                   const string& danhMuc, const double& donGia,
                   const int& soLuongTonKho, const string& maNCC,
                   const time_t& ngaySX, const int& baoHanh,
                   const bool& canThan)
    : SanPham(maSP, tenSP, danhMuc, donGia, soLuongTonKho, "CONG_NGHE", maNCC, ngaySX),
      thoiGianBaoHanh(baoHanh), canBaoQuanCanThan(canThan) {}

int CongNghe::getThoiGianBaoHanh() const {
    return thoiGianBaoHanh;
}

void CongNghe::setThoiGianBaoHanh(const int& value) {
    thoiGianBaoHanh = value;
}

bool CongNghe::getCanBaoQuanCanThan() const {
    return canBaoQuanCanThan;
}

void CongNghe::setCanBaoQuanCanThan(const bool& value) {
    canBaoQuanCanThan = value;
}

bool CongNghe::canbaoquanlanh() const {
    return false;
}

void CongNghe::inra(ostream& output) const {
    SanPham::inra(output);
    output << "\nThoi gian bao hanh: " << thoiGianBaoHanh << " thang";
    output << "\nYeu cau bao quan: "
           << (canBaoQuanCanThan ? "Can than (Chong soc, tranh am, tranh va dap manh)"
                                 : "Tieu chuan");
}

void CongNghe::docvao(istream& input) {
    SanPham::docvao(input);
}

string CongNghe::xuatchuoifile() const {
    // Dinh dang dung theo data/README.md muc 3.2 va 5.1:
    // CONG_NGHE|maSP|tenSP|danhMuc|donGia|soLuongTon|maNCC|ngaySanXuat
    return "CONG_NGHE|" + ChuoiPhanChung() + "|" + to_string(static_cast<long long>(NgaySX));
}

