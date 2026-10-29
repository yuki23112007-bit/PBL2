#include "DoGiaDung.h"

using namespace std;

DoGiaDung::DoGiaDung()
    : SanPham(), chatLieu("Inox / Hop kim") {
    LoaiSP = "DO_GIA_DUNG";
}

DoGiaDung::DoGiaDung(const DoGiaDung& other)
    : SanPham(other), chatLieu(other.chatLieu) {
    LoaiSP = "DO_GIA_DUNG";
}

DoGiaDung::DoGiaDung(const string& maSP, const string& tenSP,
                     const string& danhMuc, const double& donGia,
                     const int& soLuongTonKho, const string& maNCC,
                     const time_t& ngaySX, const string& chatLieu)
    : SanPham(maSP, tenSP, danhMuc, donGia, soLuongTonKho, "DO_GIA_DUNG", maNCC, ngaySX),
      chatLieu(chatLieu) {}

const string& DoGiaDung::getChatLieu() const {
    return chatLieu;
}

void DoGiaDung::setChatLieu(const string& value) {
    chatLieu = value;
}

bool DoGiaDung::canbaoquanlanh() const {
    return false;
}

void DoGiaDung::inra(ostream& output) const {
    SanPham::inra(output);
    output << "\nChat lieu san pham: " << chatLieu;
}

void DoGiaDung::docvao(istream& input) {
    SanPham::docvao(input);
}

string DoGiaDung::xuatchuoifile() const {
    // Dinh dang dung theo data/README.md muc 3.2 va 5.1:
    // DO_GIA_DUNG|maSP|tenSP|danhMuc|donGia|soLuongTon|maNCC|ngaySanXuat
    return "DO_GIA_DUNG|" + ChuoiPhanChung() + "|" + to_string(static_cast<long long>(NgaySX));
}

