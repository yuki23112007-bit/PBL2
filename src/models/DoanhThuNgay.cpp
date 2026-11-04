#include "DoanhThuNgay.h"
#include <iomanip>

using namespace std;

DoanhThuNgay::DoanhThuNgay()
    : ngayThongKe(""), tongDoanhThu(0.0), tongSoDonHang(0) {}

DoanhThuNgay::DoanhThuNgay(const string& ngay, double doanhThu, int soDon)
    : ngayThongKe(ngay), tongDoanhThu(doanhThu), tongSoDonHang(soDon) {}

const string& DoanhThuNgay::getNgayThongKe() const {
    return ngayThongKe;
}

double DoanhThuNgay::getTongDoanhThu() const {
    return tongDoanhThu;
}

int DoanhThuNgay::getTongSoDonHang() const {
    return tongSoDonHang;
}

void DoanhThuNgay::setNgayThongKe(const string& ngay) {
    ngayThongKe = ngay;
}

void DoanhThuNgay::setTongDoanhThu(double doanhThu) {
    tongDoanhThu = doanhThu;
}

void DoanhThuNgay::setTongSoDonHang(int soDon) {
    tongSoDonHang = soDon;
}

void DoanhThuNgay::congDonDoanhThu(double giaTriDonHang) {
    tongDoanhThu += giaTriDonHang;
    tongSoDonHang += 1;
}

void DoanhThuNgay::congDoanhThu(double giaTriDonHang) {
    congDonDoanhThu(giaTriDonHang);
}

// =========================================================================
// ĐA NĂNG HÓA TOÁN TỬ (OPERATOR OVERLOADING)
// =========================================================================

DoanhThuNgay& DoanhThuNgay::operator+=(double giaTriDonHang) {
    congDonDoanhThu(giaTriDonHang);
    return *this;
}

DoanhThuNgay& DoanhThuNgay::operator+=(const DoanhThuNgay& other) {
    this->tongDoanhThu += other.tongDoanhThu;
    this->tongSoDonHang += other.tongSoDonHang;
    return *this;
}

DoanhThuNgay DoanhThuNgay::operator+(const DoanhThuNgay& other) const {
    string ngayMoi = (this->ngayThongKe == other.ngayThongKe) ? this->ngayThongKe : (this->ngayThongKe + " ~ " + other.ngayThongKe);
    return DoanhThuNgay(ngayMoi, this->tongDoanhThu + other.tongDoanhThu, this->tongSoDonHang + other.tongSoDonHang);
}

bool DoanhThuNgay::operator==(const DoanhThuNgay& other) const {
    return this->ngayThongKe == other.ngayThongKe;
}

bool DoanhThuNgay::operator!=(const DoanhThuNgay& other) const {
    return !(*this == other);
}

bool DoanhThuNgay::operator<(const DoanhThuNgay& other) const {
    return this->ngayThongKe < other.ngayThongKe;
}

bool DoanhThuNgay::operator>(const DoanhThuNgay& other) const {
    return this->ngayThongKe > other.ngayThongKe;
}

void DoanhThuNgay::hienThiThongTin() const {
    cout << "[DOANH THU NGAY " << ngayThongKe << "]\n";
    cout << "  - Tong doanh thu : " << fixed << setprecision(0) << tongDoanhThu << " VND\n";
    cout << "  - Tong so don    : " << tongSoDonHang << " don hang\n";
}

string DoanhThuNgay::xuatChuoiFile() const {
    return ngayThongKe + "|" + to_string(static_cast<long long>(tongDoanhThu)) + "|" +
           to_string(tongSoDonHang);
}

ostream& operator<<(ostream& os, const DoanhThuNgay& dt) {
    os << dt.xuatChuoiFile();
    return os;
}

istream& operator>>(istream& is, DoanhThuNgay& dt) {
    is >> dt.ngayThongKe >> dt.tongDoanhThu >> dt.tongSoDonHang;
    return is;
}
