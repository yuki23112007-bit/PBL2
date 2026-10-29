#ifndef VANCHUYEN_H
#define VANCHUYEN_H

#include <iostream>
#include <string>
#include <ctime>

class VanChuyen {
private:
    std::string maVanChuyen;      // Khóa chính (PK, ví dụ: "VC001")
    std::string maDonHang;        // Khóa ngoại (FK, liên kết với DonHang)
    time_t ngayTao;               // Thời điểm tạo vận đơn
    std::string trangThaiGiao;    // Trạng thái: "Dang Giao", "Giao Thanh Cong", "Hoan tat tu dong"
    double chiPhiVanChuyen;       // Phí ship: 25.000đ (xe máy) hoặc 45.000đ (xe tải lạnh)
    std::string loaiXe;           // "Xe May Giao Nhanh", "Xe Tai", "Xe Tai Lanh"
    time_t thoiGianGiaoDuKien;    // Mốc thời gian dự kiến giao xong

public:
    VanChuyen();
    VanChuyen(const std::string& maVC, const std::string& maDH,
              time_t tao, const std::string& trangThai,
              double chiPhi, const std::string& xe,
              time_t duKien = 0);
    virtual ~VanChuyen() = default;

    // Getter
    const std::string& getMaVanChuyen() const;
    const std::string& getMaDonHang() const;
    time_t getNgayTao() const;
    const std::string& getTrangThaiGiao() const;
    double getChiPhiVanChuyen() const;
    const std::string& getLoaiXe() const;
    time_t getThoiGianGiaoDuKien() const;

    // Setter
    void setMaVanChuyen(const std::string& maVC);
    void setMaDonHang(const std::string& maDH);
    void setNgayTao(time_t tao);
    void setTrangThaiGiao(const std::string& trangThai);
    void setChiPhiVanChuyen(double chiPhi);
    void setLoaiXe(const std::string& xe);
    void setThoiGianGiaoDuKien(time_t duKien);

    // Nghiệp vụ (Phân hệ 1.5, 2.2, 2.6)
    void capNhatTrangThaiGiao(const std::string& trangThaiMoi);
    bool daQuaHan3Ngay(time_t thoiGianHienTai) const;
    virtual double tinhChiPhiVanChuyen() const;

    // Hiển thị & xuất dữ liệu
    virtual void hienThiThongTin() const;
    void show() const { hienThiThongTin(); }
    std::string xuatChuoiFile() const;

    // Toán tử luồng
    friend std::ostream& operator<<(std::ostream& os, const VanChuyen& vc);
    friend std::istream& operator>>(std::istream& is, VanChuyen& vc);
};

#endif // VANCHUYEN_H
