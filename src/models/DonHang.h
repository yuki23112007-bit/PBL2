#ifndef DONHANG_H
#define DONHANG_H

#include <iostream>
#include <string>
#include <ctime>

using namespace std;

class DonHang {
    private:
        string maDonHang;
        string tenKhachHang;
        string diaChiKhachHang;
        string soDienThoaiKhachHang;
        string trangThaiDonHang;
        string ngayDatHang;
        string maNguoiDung;
        time_t thoiGianTao;

    public:
        DonHang();
        DonHang(const string& maDH, const string& tenKH,
                const string& diaChi, const string& sdt,
                const string& trangThai, const string& ngayDat,
                const string& maND, time_t thoiGian = 0);
        ~DonHang() = default;

        // Getter
        const string& getMaDonHang() const;
        const string& getTenKhachHang() const;
        const string& getDiaChiKhachHang() const;
        const string& getSoDienThoaiKhachHang() const;
        const string& getTrangThaiDonHang() const;
        const string& getNgayDatHang() const;
        const string& getMaNguoiDung() const;
        time_t getThoiGianTao() const;

        // Setter
        void setMaDonHang(const string&);
        void setTenKhachHang(const string&);
        void setDiaChiKhachHang(const string&);
        void setSoDienThoaiKhachHang(const string&);
        void setTrangThaiDonHang(const string&);
        void setNgayDatHang(const string&);
        void setMaNguoiDung(const string&);
        void setThoiGianTao(time_t);

        bool daQuaHan5Phut(time_t thoiGianHienTai) const;
        bool daQuaHan2Phut(time_t thoiGianHienTai) const;

        void hienThiThongTin() const;
        void show() const { hienThiThongTin(); }
        string xuatChuoiFile() const;

        friend ostream& operator<<(ostream&, const DonHang&);
        friend istream& operator>>(istream&, DonHang&);
};

#endif // DONHANG_H
