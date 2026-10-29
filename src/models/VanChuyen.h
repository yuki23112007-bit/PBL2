#ifndef VANCHUYEN_H
#define VANCHUYEN_H

#include <iostream>
#include <string>
#include <ctime>

using namespace std;

class VanChuyen {
private:
    string maVanChuyen;      // Khóa chính (PK, ví dụ: "VC001")
    string maDonHang;        // Khóa ngoại (FK, liên kết với DonHang)
    time_t ngayTao;          // Thời điểm tạo vận đơn
    string trangThaiGiao;    // Trạng thái: "Dang Giao", "Giao Thanh Cong", "Hoan tat tu dong"
    double chiPhiVanChuyen;  // Phí ship: 25.000đ (xe máy) hoặc 45.000đ (xe tải lạnh)
    string loaiXe;           // "Xe May Giao Nhanh", "Xe Tai", "Xe Tai Lanh"
    time_t thoiGianGiaoDuKien; // Mốc thời gian dự kiến giao xong

public:
    VanChuyen();
    VanChuyen(const string& maVC, const string& maDH,
              time_t tao, const string& trangThai,
              double chiPhi, const string& xe,
              time_t duKien = 0);
    virtual ~VanChuyen() = default;

    // Getter
    const string& getMaVanChuyen() const;
    const string& getMaDonHang() const;
    time_t getNgayTao() const;
    const string& getTrangThaiGiao() const;
    double getChiPhiVanChuyen() const;
    const string& getLoaiXe() const;
    time_t getThoiGianGiaoDuKien() const;

    // Setter
    void setMaVanChuyen(const string& maVC);
    void setMaDonHang(const string& maDH);
    void setNgayTao(time_t tao);
    void setTrangThaiGiao(const string& trangThai);
    void setChiPhiVanChuyen(double chiPhi);
    void setLoaiXe(const string& xe);
    void setThoiGianGiaoDuKien(time_t duKien);

    // Nghiệp vụ (Phân hệ 1.5, 2.2, 2.6)
    void capNhatTrangThaiGiao(const string& trangThaiMoi);
    bool daQuaHan3Ngay(time_t thoiGianHienTai) const;
    virtual double tinhChiPhiVanChuyen() const;

    // Hiển thị & xuất dữ liệu
    virtual void hienThiThongTin() const;
    void show() const { hienThiThongTin(); }
    string xuatChuoiFile() const;

    // Toán tử luồng
    friend ostream& operator<<(ostream& os, const VanChuyen& vc);
    friend istream& operator>>(istream& is, VanChuyen& vc);
};

#endif // VANCHUYEN_H
