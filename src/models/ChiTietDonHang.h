#ifndef CHITIETDONHANG_H
#define CHITIETDONHANG_H

#include <iostream>
#include <string>

using namespace std;

class ChiTietDonHang {
    private:
        string maChiTiet;
        string maDonHang;
        string maSanPham;
        int soLuong;
        double thanhTien;

    public:
        ChiTietDonHang();
        ChiTietDonHang(const string& maCT, const string& maDH,
                       const string& maSP, int soLuong, double donGia = 0.0);
        ~ChiTietDonHang() = default;

        // Getter
        const string& getMaChiTiet() const;
        const string& getMaDonHang() const;
        const string& getMaSanPham() const;
        int getSoLuong() const;
        double getThanhTien() const;

        // Setter
        void setMaChiTiet(const string&);
        void setMaDonHang(const string&);
        void setMaSanPham(const string&);
        void setSoLuong(int);
        void setThanhTien(double);

        void tinhThanhTien(double donGia);
        void hienThiThongTin() const;
        void show() const { hienThiThongTin(); }
        string xuatChuoiFile() const;

        friend ostream& operator<<(ostream&, const ChiTietDonHang&);
        friend istream& operator>>(istream&, ChiTietDonHang&);
};

#endif // CHITIETDONHANG_H
