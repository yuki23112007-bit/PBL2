#ifndef QUANLYVANCHUYEN_H
#define QUANLYVANCHUYEN_H

#include <string>
#include <vector>
#include <ctime>

#include "../models/DonHang.h"
#include "../models/ChiTietDonHang.h"
#include "QuanLySanPham.h"
#include "QuanLyKhoNCC.h"

using namespace std;

struct UocTinhVanChuyen {
    string maKho;
    string thanhPhoKho;
    string thanhPhoKhach;
    double quangDuongKm;
    string loaiXe;
    double phiVanChuyen;
    long long soGiayGiaoDuKien;
    string nhanThoiGianGiao;
};

// Lop nghiep vu van chuyen (SRP):
// - Chon kho xuat hang phu hop
// - Dieu phoi phuong tien da hinh & tinh phi ship (qua VanChuyen / PhuongTien)
// - Tinh ngay / thoi gian giao hang du kien theo khoang cach vung
class QuanLyVanChuyen {
public:
    static long long tinhSoGiayGiaoDuKien(double quangDuongKm, bool coHangTuoiSong, string& nhanThoiGianGiao);
    static double tinhPhiVanChuyenTheoXe(double quangDuongKm, double tienHang, bool coHangTuoiSong, string& loaiXeRa);

    static UocTinhVanChuyen uocTinhChoSanPham(const string& maSP,
                                              double tienHang,
                                              const string& diaChiKhach,
                                              QuanLySanPham& qlSanPham,
                                              const QuanLyKhoNCC& qlKhoNCC);

    static UocTinhVanChuyen uocTinhChoDonHang(const DonHang* dh,
                                              const vector<ChiTietDonHang>& dsCTDH,
                                              QuanLySanPham& qlSanPham,
                                              const QuanLyKhoNCC& qlKhoNCC);
};

#endif // QUANLYVANCHUYEN_H
