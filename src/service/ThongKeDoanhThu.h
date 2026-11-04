#ifndef THONGKEDOANHTHU_H
#define THONGKEDOANHTHU_H

#include "../models/DoanhThuNgay.h"
#include "../core/ThuatToan.h"
#include <vector>
#include <string>

using namespace std;

// Ket qua phan tich xu huong bang thuat toan hoi quy tuyen tinh OLS (Ordinary Least Squares)
struct KetQuaXuHuong {
    double heSoGoc;              // a: do doc xu huong (VND/ngay)
    double heSoChan;             // b: he so chan hoi quy
    string nhanXuHuong;          // "Tăng trưởng ↗", "Suy giảm ↘", "Bình ổn →"
    double tocDoTangTruong;      // Ty le tang truong giua ngay cuoi va ngay dau (%)
    double duBaoNgayKe;          // Doanh thu du bao cho ngay ke tiep (VND)
    vector<double> duongXuHuong; // Cac diem gia tri tren duong xu huong tai tung ngay
};

// Lop thuc the thuat toan thong ke & phan tich xu huong doanh thu
class ThongKeDoanhThu {
private:
    vector<DoanhThuNgay> danhSach;

public:
    ThongKeDoanhThu() = default;
    explicit ThongKeDoanhThu(const vector<DoanhThuNgay>& ds);
    virtual ~ThongKeDoanhThu() = default;

    void napDuLieu(const vector<DoanhThuNgay>& ds);
    void themBanGhi(const DoanhThuNgay& dt);

    // 1. Thuat toan thong ke mo ta
    double tinhTongDoanhThu() const;
    int tinhTongSoDonHang() const;
    int soNgayThongKe() const;
    double tinhTrungBinhNgay() const;

    // 2. Thuat toan sap xep QuickSort (dung ThuatToan::quickSort) de xac dinh gia tri bien va trung vi
    double timDoanhThuCaoNhat(string& ngayDatDinh) const;
    double timDoanhThuThapNhat(string& ngayDay) const;
    double tinhTrungViDoanhThu() const; // Median loai bo nhieu do ngay dot bien

    // 3. Thuat toan hoi quy tuyen tinh phan tich xu huong (Linear Regression Trend Analysis)
    KetQuaXuHuong phanTichXuHuong() const;
};

#endif // THONGKEDOANHTHU_H
