#ifndef NHACUNGCAP_H
#define NHACUNGCAP_H

#include <iostream>
#include <string>

using namespace std;

class NhaCungCap {
private:
    string maNhaCungCap;    // PK, ví dụ: NCC01
    string tenCongTy;       // Tên doanh nghiệp cung cấp
    string diaChi;          // Địa chỉ trụ sở / kho bãi
    string soDienThoai;     // Số điện thoại liên lạc
    string danhMucCungCap;  // Danh mục mặt hàng cung ứng

public:
    NhaCungCap();
    NhaCungCap(const string& maNCC, const string& tenCT,
               const string& diaChi, const string& sdt,
               const string& danhMuc);
    virtual ~NhaCungCap() = default;

    // Getter
    const string& getMaNhaCungCap() const;
    const string& getTenCongTy() const;
    const string& getDiaChi() const;
    const string& getSoDienThoai() const;
    const string& getDanhMucCungCap() const;

    // Bí danh phù hợp tài liệu
    const string& getMaNCC() const { return getMaNhaCungCap(); }

    // Setter
    void setMaNhaCungCap(const string& ma);
    void setTenCongTy(const string& ten);
    void setDiaChi(const string& dc);
    void setSoDienThoai(const string& sdt);
    void setDanhMucCungCap(const string& dm);

    // Hiển thị & xuất chuỗi file
    virtual void hienThiThongTin() const;
    void show() const { hienThiThongTin(); }
    string xuatChuoiFile() const;

    // Toán tử luồng
    friend ostream& operator<<(ostream& os, const NhaCungCap& ncc);
    friend istream& operator>>(istream& is, NhaCungCap& ncc);
};

#endif // NHACUNGCAP_H
