#include "DoanhThuNgay.h"
#include <iomanip>

DoanhThuNgay::DoanhThuNgay()
    : ngayThongKe(""), tongDoanhThu(0.0), tongSoDonHang(0) {}

DoanhThuNgay::DoanhThuNgay(const std::string& ngay, double doanhThu, int soDon)
    : ngayThongKe(ngay), tongDoanhThu(doanhThu), tongSoDonHang(soDon) {}

const std::string& DoanhThuNgay::getNgayThongKe() const {
    return ngayThongKe;
}

double DoanhThuNgay::getTongDoanhThu() const {
    return tongDoanhThu;
}

int DoanhThuNgay::getTongSoDonHang() const {
    return tongSoDonHang;
}

void DoanhThuNgay::setNgayThongKe(const std::string& ngay) {
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

void DoanhThuNgay::hienThiThongTin() const {
    std::cout << "[DOANH THU NGAY " << ngayThongKe << "]\n";
    std::cout << "  - Tong doanh thu : " << std::fixed << std::setprecision(0) << tongDoanhThu << " VND\n";
    std::cout << "  - Tong so don    : " << tongSoDonHang << " don hang\n";
}

std::string DoanhThuNgay::xuatChuoiFile() const {
    return ngayThongKe + "|" + std::to_string(static_cast<long long>(tongDoanhThu)) + "|" +
           std::to_string(tongSoDonHang);
}

std::ostream& operator<<(std::ostream& os, const DoanhThuNgay& dt) {
    os << dt.xuatChuoiFile();
    return os;
}

std::istream& operator>>(std::istream& is, DoanhThuNgay& dt) {
    is >> dt.ngayThongKe >> dt.tongDoanhThu >> dt.tongSoDonHang;
    return is;
}
