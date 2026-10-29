#ifndef SANPHAM_H
#define SANPHAM_H

#include <iostream>
#include <string>
#include <ctime>
using namespace std;

class SanPham {
    protected:
        string MaSP;
        string TenSP;
        string DandMuc;
        double DonGia;
        int SoLuongTonKho;
        string LoaiSP;
        string MaNCC;
        time_t NgaySX;
        int HanDungSoNgay;

        // ham ho tro lop con
        string ChuoiPhanChung() const;  // "ma|ten|danhMuc|donGia|soLuongTon|maNCC"
        string HanSuDung() const;       // "ngaySanXuat|hanDungSoNgay" (luon o CUOI dong)

        static string SoThanhChuoi(const double&);

        virtual void inra(ostream&) const;
        virtual void docvao(istream&);
    public:
        // constructor & destructor
        SanPham();
        SanPham(const SanPham&);
        SanPham(const string&, const string&, const string&, const double&, const int&, const string&, const string&, const time_t&, const int&);
        virtual ~SanPham();

        // getter
        const string& getMaSP() const;
        const string& getTenSP() const;
        const string& getDanhMuc() const;
        double        getDonGia() const;
        int           getSoLuongTonKho() const;
        const string  getLoaiSP() const;
        const string  getMaNCC() const;
        time_t        getNgaySX() const;
        int           getHanDungSoNgay() const;

        // setter
        void setMaSP(const string&);
        void setTenSP(const string&);
        void setDanhMuc(const string&);
        void setDonGia(const double&);
        void setSoLuongTonKho(const int&);
        void setLoaiSP(const string&);
        void setMaNCC(const string&);
        void setNgaySX(const time_t&);
        void setHanDungSoNgay(const int&);

        // nghiep vu
        bool CapNhatSoLuongTonKho(const int&);    // neu so - se khong doi
        bool KiemTranCanhBaoHetHang(const int& = 10) const;
        time_t ngayhethang() const;    // = ngaysx + hsd * 86400

        // da hinh
        virtual void show() const;
        virtual bool canbaoquanlanh() const;
        virtual string xuatchuoifile() const = 0;

        // Bi danh phu hop voi tai lieu Word
        const string& getMaSanPham() const { return getMaSP(); }
        const string& getTenSanPham() const { return getTenSP(); }
        int getSoLuongTon() const { return getSoLuongTonKho(); }
        void setSoLuongTon(int v) { setSoLuongTonKho(v); }
        void capNhatSoLuongTon(int v) { CapNhatSoLuongTonKho(v); }
        bool kiemTraCanhBaoHetHang(int nguong = 10) const { return KiemTranCanhBaoHetHang(nguong); }
        time_t ngayHetHan() const { return ngayhethang(); }
        bool canBaoQuanLanh() const { return canbaoquanlanh(); }
        string xuatChuoiFile() const { return xuatchuoifile(); }
        void hienThiThongTin() const { show(); }

        friend ostream& operator<<(ostream&, const SanPham&);
        friend istream& operator>>(istream&, SanPham&);
};

#endif // SANPHAM_H
