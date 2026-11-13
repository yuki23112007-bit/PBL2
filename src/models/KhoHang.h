#ifndef KHOHANG_H
#define KHOHANG_H

#include <iostream>
#include <string>
#include <string>

using namespace std;

class KhoHang {
    private:
        string maKho;
        string viTri;
        int sucChuaToiDa;
        string maSanPham;

    public:
        KhoHang();
        KhoHang(const string& maKho, const string& viTri,
                int sucChua, const string& maSP);
        ~KhoHang() = default;

        // Getter
        const string& getMaKho() const;
        const string& getViTri() const;
        int getSucChuaToiDa() const;
        const string& getMaSanPham() const;

        // Setter
        void setViTri(const string&);
        void setSucChuaToiDa(int);
        void setMaSanPham(const string&);

        // Nghiep vu
        void capNhatThongTin(const string& viTriMoi, int sucChuaMoi, const string& maSPMoi);

        void hienThiThongTin() const;
        void show() const { hienThiThongTin(); }
        string xuatChuoiFile() const;

        friend ostream& operator<<(ostream&, const KhoHang&);
        friend istream& operator>>(istream&, KhoHang&);
};

#endif // KHOHANG_H
