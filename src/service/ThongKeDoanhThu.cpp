#include "ThongKeDoanhThu.h"
#include <cmath>

using namespace std;

ThongKeDoanhThu::ThongKeDoanhThu(const vector<DoanhThuNgay>& ds)
    : danhSach(ds) {}

void ThongKeDoanhThu::napDuLieu(const vector<DoanhThuNgay>& ds) {
    danhSach = ds;
}

void ThongKeDoanhThu::themBanGhi(const DoanhThuNgay& dt) {
    danhSach.push_back(dt);
}

double ThongKeDoanhThu::tinhTongDoanhThu() const {
    double tong = 0.0;
    for (const auto& dt : danhSach) {
        tong += dt.getTongDoanhThu();
    }
    return tong;
}

int ThongKeDoanhThu::tinhTongSoDonHang() const {
    int tong = 0;
    for (const auto& dt : danhSach) {
        tong += dt.getTongSoDonHang();
    }
    return tong;
}

int ThongKeDoanhThu::soNgayThongKe() const {
    return (int)danhSach.size();
}

double ThongKeDoanhThu::tinhTrungBinhNgay() const {
    if (danhSach.empty()) return 0.0;
    return tinhTongDoanhThu() / (double)danhSach.size();
}

double ThongKeDoanhThu::timDoanhThuCaoNhat(string& ngayDatDinh) const {
    if (danhSach.empty()) {
        ngayDatDinh = "";
        return 0.0;
    }
    double maxVal = danhSach[0].getTongDoanhThu();
    ngayDatDinh = danhSach[0].getNgayThongKe();
    for (size_t i = 1; i < danhSach.size(); i++) {
        if (danhSach[i].getTongDoanhThu() > maxVal) {
            maxVal = danhSach[i].getTongDoanhThu();
            ngayDatDinh = danhSach[i].getNgayThongKe();
        }
    }
    return maxVal;
}

double ThongKeDoanhThu::timDoanhThuThapNhat(string& ngayDay) const {
    if (danhSach.empty()) {
        ngayDay = "";
        return 0.0;
    }
    double minVal = danhSach[0].getTongDoanhThu();
    ngayDay = danhSach[0].getNgayThongKe();
    for (size_t i = 1; i < danhSach.size(); i++) {
        if (danhSach[i].getTongDoanhThu() < minVal) {
            minVal = danhSach[i].getTongDoanhThu();
            ngayDay = danhSach[i].getNgayThongKe();
        }
    }
    return minVal;
}

// Su dung thuat toan QuickSort tu ThuatToan::quickSort de sap xep mang doanh thu va tinh trung vi (Median)
double ThongKeDoanhThu::tinhTrungViDoanhThu() const {
    if (danhSach.empty()) return 0.0;
    vector<double> vals;
    vals.reserve(danhSach.size());
    for (const auto& dt : danhSach) {
        vals.push_back(dt.getTongDoanhThu());
    }

    ThuatToan::quickSort(vals, 0, (int)vals.size() - 1, true);

    int n = (int)vals.size();
    if (n % 2 != 0) {
        return vals[n / 2];
    } else {
        return (vals[(n / 2) - 1] + vals[n / 2]) / 2.0;
    }
}

// Thuat toan hoi quy tuyen tinh OLS (Ordinary Least Squares) de tinh duong xu huong: y = ax + b
KetQuaXuHuong ThongKeDoanhThu::phanTichXuHuong() const {
    KetQuaXuHuong kq;
    kq.heSoGoc = 0.0;
    kq.heSoChan = 0.0;
    kq.nhanXuHuong = "Bình ổn →";
    kq.tocDoTangTruong = 0.0;
    kq.duBaoNgayKe = 0.0;

    int n = (int)danhSach.size();
    if (n == 0) return kq;

    if (n == 1) {
        double y = danhSach[0].getTongDoanhThu();
        kq.heSoChan = y;
        kq.duBaoNgayKe = y;
        kq.duongXuHuong.push_back(y);
        return kq;
    }

    double sumX = 0.0;
    double sumY = 0.0;
    double sumXY = 0.0;
    double sumX2 = 0.0;

    for (int i = 0; i < n; i++) {
        double x = (double)(i + 1); // moc thoi gian 1, 2, ..., n
        double y = danhSach[i].getTongDoanhThu();
        sumX += x;
        sumY += y;
        sumXY += (x * y);
        sumX2 += (x * x);
    }

    double denom = ((double)n * sumX2) - (sumX * sumX);
    if (abs(denom) > 1e-9) {
        kq.heSoGoc = (((double)n * sumXY) - (sumX * sumY)) / denom;
        kq.heSoChan = (sumY - (kq.heSoGoc * sumX)) / (double)n;
    } else {
        kq.heSoGoc = 0.0;
        kq.heSoChan = sumY / (double)n;
    }

    // Tinh cac diem tren duong xu huong
    kq.duongXuHuong.reserve(n);
    for (int i = 0; i < n; i++) {
        double x = (double)(i + 1);
        double yVal = (kq.heSoGoc * x) + kq.heSoChan;
        if (yVal < 0) yVal = 0;
        kq.duongXuHuong.push_back(yVal);
    }

    // Du bao ngay tiep theo (x = n + 1)
    double duBao = (kq.heSoGoc * (double)(n + 1)) + kq.heSoChan;
    kq.duBaoNgayKe = (duBao < 0) ? 0.0 : duBao;

    // Ty le tang truong tong the
    double yDau = danhSach[0].getTongDoanhThu();
    double yCuoi = danhSach[n - 1].getTongDoanhThu();
    if (yDau > 0) {
        kq.tocDoTangTruong = ((yCuoi - yDau) / yDau) * 100.0;
    }

    // Nhan xet xu huong
    if (kq.heSoGoc > 20000.0) {
        kq.nhanXuHuong = "Tăng trưởng ↗";
    } else if (kq.heSoGoc < -20000.0) {
        kq.nhanXuHuong = "Suy giảm ↘";
    } else {
        kq.nhanXuHuong = "Bình ổn →";
    }

    return kq;
}
