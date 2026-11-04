#ifndef PHUONGTIEN_H
#define PHUONGTIEN_H

#include <iostream>
#include <string>

using namespace std;

// =========================================================================
// TẦNG 1: LỚP CƠ SỞ TRỪU TƯỢNG PHƯƠNG TIỆN (Abstract Base Class)
// =========================================================================
class PhuongTien {
protected:
    string bienSo;         // Biển số xe (ví dụ: "43C-12345")
    string hangSanXuat;    // Hãng sản xuất (ví dụ: "Hino", "Isuzu", "Honda")
    string loaiPhuongTien; // Tên phân loại phương tiện

public:
    PhuongTien();
    PhuongTien(const string& bienSo, const string& hangSX, const string& loaiPT);
    virtual ~PhuongTien() = default;

    // Getter
    const string& getBienSo() const;
    const string& getHangSanXuat() const;
    const string& getLoaiPhuongTien() const;

    // Setter
    void setBienSo(const string& bs);
    void setHangSanXuat(const string& hang);
    void setLoaiPhuongTien(const string& loai);

    // Tính đa hình (Polymorphism): Mỗi loại phương tiện tính giá cước vận chuyển khác nhau
    virtual double tinhChiPhiVanChuyen(double quangDuongKm = 1.0) const = 0;
    virtual void hienThiThongTin() const;
    void show() const { hienThiThongTin(); }
};

// =========================================================================
// TẦNG 2: LỚP TRUNG GIAN XE TẢI (Kế thừa PhuongTien)
// =========================================================================
class XeTai : public PhuongTien {
protected:
    double trongTaiToiDa; // Tải trọng tối đa tính bằng tấn (ví dụ: 2.5 tấn)

public:
    XeTai();
    XeTai(const string& bienSo, const string& hangSX, double trongTai);
    ~XeTai() override = default;

    double getTrongTaiToiDa() const;
    void setTrongTaiToiDa(double trongTai);

    // Giá xe tải thông thường: 15.000đ/km theo tài liệu
    double tinhChiPhiVanChuyen(double quangDuongKm = 1.0) const override;
    void hienThiThongTin() const override;

    virtual void load();
    virtual void unload();
};

// =========================================================================
// TẦNG 3: LỚP CON CHUYÊN BIỆT XE TẢI LẠNH (Kế thừa nhiều tầng từ XeTai)
// =========================================================================
class XeTaiLanh : public XeTai {
private:
    double nhietDoThungXe; // Nhiệt độ thùng đông lạnh (°C, ví dụ: -18.0°C để chở đồ tươi sống)

public:
    XeTaiLanh();
    XeTaiLanh(const string& bienSo, const string& hangSX, double trongTai, double nhietDo);
    ~XeTaiLanh() override = default;

    double getNhietDoThungXe() const;
    void setNhietDoThungXe(double nhietDo);

    // Ghi đè (Override): Xe tải lạnh tính giá 25.000đ/km (tốn tiền chạy hệ thống làm lạnh)
    double tinhChiPhiVanChuyen(double quangDuongKm = 1.0) const override;
    void hienThiThongTin() const override;
};

// =========================================================================
// LỚP XE MÁY GIAO NHANH (Kế thừa PhuongTien)
// =========================================================================
class XeMay : public PhuongTien {
private:
    int dungTichXiLanh; // Dung tích xi lanh (cc, ví dụ: 125cc)

public:
    XeMay();
    XeMay(const string& bienSo, const string& hangSX, int dungTich = 125);
    ~XeMay() override = default;

    int getDungTichXiLanh() const;
    void setDungTichXiLanh(int dungTich);

    // Giá ship xe máy giao nhanh: 10.000đ/km hoặc mặc định cước nội thành 25.000đ
    double tinhChiPhiVanChuyen(double quangDuongKm = 1.0) const override;
    void hienThiThongTin() const override;
};

#endif // PHUONGTIEN_H
