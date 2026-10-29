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
        time_t NgaySX; // Moi san pham deu co ngay san xuat

        // Ham ho tro lop con
        string ChuoiPhanChung() const; // "ma|ten|danhMuc|donGia|soLuongTon|maNCC"
        static string SoThanhChuoi(const double&);

        virtual void inra(ostream&) const;
        virtual void docvao(istream&);

    public:
        // Constructor & Destructor
        SanPham();
        SanPham(const SanPham&);
        SanPham(const string& maSP, const string& tenSP, const string& danhMuc,
                const double& donGia, const int& soLuongTonKho, const string& loaiSP,
                const string& maNCC, const time_t& ngaySX);
        virtual ~SanPham();

        // Getter
        const string& getMaSP() const;
        const string& getTenSP() const;
        const string& getDanhMuc() const;
        double        getDonGia() const;
        int           getSoLuongTonKho() const;
        const string  getLoaiSP() const;
        const string  getMaNCC() const;
        time_t        getNgaySX() const;

        // Setter
        void setMaSP(const string&);
        void setTenSP(const string&);
        void setDanhMuc(const string&);
        void setDonGia(const double&);
        void setSoLuongTonKho(const int&);
        void setLoaiSP(const string&);
        void setMaNCC(const string&);
        void setNgaySX(const time_t&);

        // Nghiep vu
        bool CapNhatSoLuongTonKho(const int&);
        bool KiemTranCanhBaoHetHang(const int& = 10) const;

        // Da hinh
        virtual void show() const;
        virtual bool canbaoquanlanh() const;
        virtual string xuatchuoifile() const = 0;

        // Bi danh phu hop voi tai lieu
        const string& getMaSanPham() const { return getMaSP(); }
        const string& getTenSanPham() const { return getTenSP(); }
        int getSoLuongTon() const { return getSoLuongTonKho(); }
        void setSoLuongTon(int v) { setSoLuongTonKho(v); }
        void capNhatSoLuongTon(int v) { CapNhatSoLuongTonKho(v); }
        bool kiemTraCanhBaoHetHang(int nguong = 10) const { return KiemTranCanhBaoHetHang(nguong); }
        bool canBaoQuanLanh() const { return canbaoquanlanh(); }
        string xuatChuoiFile() const { return xuatchuoifile(); }
        void hienThiThongTin() const { show(); }

        friend ostream& operator<<(ostream&, const SanPham&);
        friend istream& operator>>(istream&, SanPham&);
};

#endif // SANPHAM_H
