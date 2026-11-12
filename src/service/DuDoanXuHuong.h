#ifndef DU_DOAN_XU_HUONG_H
#define DU_DOAN_XU_HUONG_H

#include "../core/GaussianNaiveBayes.h"
#include "../service/QuanLyDonHang.h"
#include "../service/QuanLySanPham.h"
#include "../models/TaiKhoan.h"
#include "../models/DanhGiaDonHang.h"
#include <string>
#include <vector>
#include <map>

// ============================================================================
// Cau truc du lieu ket qua du doan
// ============================================================================
struct KetQuaQuayLai {
    std::string idKhachHang;
    std::string hoTen;
    std::string username;
    double xacSuatQuayLai;      // Ty le % (0.0 -> 100.0)
    double recencyNgay;         // x1: So ngay tu don cuoi
    int frequencyDon;           // x2: So don hoan thanh
    double monetaryAOV;         // x3: Chi tieu trung binh / don
    double satisfactionSao;     // x4: Diem sao danh gia trung binh
    std::string mucDo;          // "Cao", "Trung bình", "Thấp"
};

struct KetQuaNhomSP {
    std::string idKhachHang;
    std::string hoTen;
    std::string username;
    std::string nhomDuDoan;     // "Thực phẩm", "Công nghệ", "Đồ gia dụng"
    double xacSuatNhom;         // Ty le % (0.0 -> 100.0)
    std::vector<double> xacSuatCacNhom; // [P(ThucPham), P(CongNghe), P(GiaDung)]
};

// ============================================================================
// Lop nghiep vu DuDoanXuHuong (Tinh nang 19 - Machine Learning Gaussian Naive Bayes)
// ============================================================================
class DuDoanXuHuong {
private:
    GaussianNaiveBayes modelQuayLai; // Phuong an 1: 2 lop (0: Khong quay lai, 1: Quay lai)
    GaussianNaiveBayes modelNhomSP;  // Phuong an 2: 3 lop (0: Thuc pham, 1: Cong nghe, 2: Do gia dung)

    QuanLyDonHang* qlDonHang;
    const std::vector<TaiKhoan*>* danhSachTaiKhoan;
    QuanLySanPham* qlSanPham;

    std::map<std::string, double> diemDanhGiaMap; // maDonHang -> soSao (1..5)
    int soLuongMauHuanLuyen;
    time_t thoiGianHuanLuyen;

    // QuickSort sap xep giam dan theo xac suat quay lai
    void quickSortKetQua(std::vector<KetQuaQuayLai>& arr, int low, int high);

public:
    DuDoanXuHuong();
    DuDoanXuHuong(QuanLyDonHang* qlDH, const std::vector<TaiKhoan*>* dsTK, QuanLySanPham* qlSP);
    ~DuDoanXuHuong();

    void ganDependencies(QuanLyDonHang* qlDH, const std::vector<TaiKhoan*>* dsTK, QuanLySanPham* qlSP);
    void dongBoDanhGia(const std::vector<DanhGiaDonHang>& dsDG);
    void capNhatDanhGia(const std::string& maDH, double soSao);

    // Huan luyen lai ca 2 mo hinh Naive Bayes tu du lieu don hang
    bool huanLuyen();

    // API Admin: Danh sach du doan khach quay lai (sap xep giam dan xac suat bang QuickSort)
    std::vector<KetQuaQuayLai> duDoanTatCaQuayLai();

    // API Admin: Danh sach du doan nhom san pham tiep theo
    std::vector<KetQuaNhomSP> duDoanTatCaNhomSP();

    // API Customer: Du doan cho rieng tai khoan dang dang nhap
    bool duDoanChoKhachHang(const std::string& idKhachHang, KetQuaQuayLai& kqQuayLai, KetQuaNhomSP& kqNhom);

    int getSoLuongMau() const { return soLuongMauHuanLuyen; }
    time_t getThoiGianHuanLuyen() const { return thoiGianHuanLuyen; }
};

#endif // DU_DOAN_XU_HUONG_H
