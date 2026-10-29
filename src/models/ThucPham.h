#ifndef THUCPHAM_H
#define THUCPHAM_H

#include "SanPham.h"
#include <iostream>
#include <string>
#include <ctime>

using namespace std;

class ThucPham : public SanPham {
    protected:
        int HanDungSoNgay; // Han su dung tinh bang so ngay

        void inra(ostream&) const override;
        void docvao(istream&) override;
        string HanSuDung() const;

    public:
        ThucPham();
        ThucPham(const ThucPham&);
        ThucPham(const string& maSP, const string& tenSP, const string& danhMuc,
                 const double& donGia, const int& soLuongTonKho, const string& loaiSP,
                 const string& maNCC, const time_t& ngaySX, const int& hanDungSoNgay);
        ~ThucPham() override;

        int getHanDungSoNgay() const;
        void setHanDungSoNgay(const int&);

        // Tinh toan thoi diem het han theo tai lieu
        time_t ngayhethang() const;
        time_t ngayHetHan() const { return ngayhethang(); }
        bool daHetHan(time_t thoiDiemHienTai = 0) const;
        double soNgayConLai(time_t thoiDiemHienTai = 0) const;

        // Bi danh phu hop tai lieu
        int getHanDung() const { return getHanDungSoNgay(); }
        void setHanDung(int v) { setHanDungSoNgay(v); }
};

#endif // THUCPHAM_H

