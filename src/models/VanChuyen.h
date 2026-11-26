#ifndef VANCHUYEN_H
#define VANCHUYEN_H

#include <string>
#include <ctime>

using namespace std;

// Khai báo chuyển tiếp (Forward Declarations)
class DonHang;
class PhuongTien;

// Lop thuc the van don (SRP): chi quan ly trang thai van don va tinh cuoc da hinh qua PhuongTien*
class VanChuyen {
private:
    string maVanChuyen;        // Khóa chính (PK, ví dụ: "VC001")
    string maDonHang;          // Khóa ngoại (FK, liên kết với DonHang)
    time_t ngayTao;            // Thời điểm tạo vận đơn
    string trangThaiGiao;      // Trạng thái: "Dang Giao", "Giao Thanh Cong", "Hoan tat tu dong"
    double chiPhiVanChuyen;    // Phí vận chuyển tính theo phương tiện & quãng đường
    string loaiXe;             // "Xe May Giao Nhanh", "Xe Tai", "Xe Tai Lanh Chuyen Dung"
    time_t thoiGianGiaoDuKien; // Mốc thời gian dự kiến giao xong
    PhuongTien* phuongTien;    // Con trỏ phương tiện điều phối thực tế (Đa hình OOP)

public:
    VanChuyen();
    VanChuyen(const string& maVC, const string& maDH,
              time_t tao, const string& trangThai,
              double chiPhi, const string& xe,
              time_t duKien = 0, PhuongTien* pt = nullptr);
    virtual ~VanChuyen() = default;

    // Getter
    const string& getMaVanChuyen() const;
    const string& getMaDonHang() const;
    time_t getNgayTao() const;
    const string& getTrangThaiGiao() const;
    double getChiPhiVanChuyen() const;
    const string& getLoaiXe() const;
    time_t getThoiGianGiaoDuKien() const;
    PhuongTien* getPhuongTien() const;

    // Setter (khong cho doi maVanChuyen, maDonHang bat bien)
    void setNgayTao(time_t tao);
    void setTrangThaiGiao(const string& trangThai);
    void setChiPhiVanChuyen(double chiPhi);
    void setLoaiXe(const string& xe);
    void setThoiGianGiaoDuKien(time_t duKien);
    void ganPhuongTien(PhuongTien* pt);

    // Nghiệp vụ trạng thái vận đơn & tính cước đa hình
    void capNhatTrangThaiGiao(const string& trangThaiMoi);
    bool daQuaHan3Ngay(time_t thoiGianHienTai) const;
    virtual double tinhChiPhiVanChuyen(double quangDuongKm = 1.0);
};

#endif // VANCHUYEN_H
