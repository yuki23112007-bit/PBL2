#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

#include "../models/SanPham.h"
#include "../models/ThucPham.h"
#include "../models/ThucPhamThuong.h"
#include "../models/ThucPhamTuoiSong.h"
#include "../models/CongNghe.h"
#include "../models/DoGiaDung.h"
#include "../models/KhoHang.h"
#include "../models/DonHang.h"
#include "../models/ChiTietDonHang.h"
#include "../models/TaiKhoan.h"
#include "../models/Admin.h"
#include "../models/NguoiDung.h"

using namespace std;

// Bí danh để tương thích ngược nếu có module cũ gọi tên SanPhamThuong / SanPhamTuoiSong
using SanPhamThuong = ThucPhamThuong;
using SanPhamTuoiSong = ThucPhamTuoiSong;

// Cấu trúc đại diện cho bản ghi nhà cung cấp (suppliers.txt)
struct NhaCungCapRecord {
    string maNhaCungCap;
    string tenCongTy;
    string diaChi;
    string soDienThoai;
    string danhMucCungCap;

    NhaCungCapRecord()
        : maNhaCungCap(""), tenCongTy(""), diaChi(""), soDienThoai(""), danhMucCungCap("") {}

    NhaCungCapRecord(const string& ma, const string& ten, const string& diaChi,
                     const string& sdt, const string& danhMuc)
        : maNhaCungCap(ma), tenCongTy(ten), diaChi(diaChi),
          soDienThoai(sdt), danhMucCungCap(danhMuc) {}

    string xuatChuoiFile() const {
        return maNhaCungCap + "|" + tenCongTy + "|" + diaChi + "|" +
               soDienThoai + "|" + danhMucCungCap;
    }
};

// Cấu trúc đại diện cho bản ghi doanh thu theo ngày (revenues.txt)
struct DoanhThuRecord {
    string ngayThongKe;  // Định dạng yyyy-mm-dd
    double tongDoanhThu;
    int tongSoDonHang;

    DoanhThuRecord()
        : ngayThongKe(""), tongDoanhThu(0.0), tongSoDonHang(0) {}

    DoanhThuRecord(const string& ngay, double doanhThu, int soDon)
        : ngayThongKe(ngay), tongDoanhThu(doanhThu), tongSoDonHang(soDon) {}

    string xuatChuoiFile() const {
        return ngayThongKe + "|" + to_string(static_cast<long long>(tongDoanhThu)) + "|" +
               to_string(tongSoDonHang);
    }
};

// Cấu trúc DTO cho User (giữ tương thích)
struct UserRecord {
    string loai;
    string id;
    string username;
    string passwordHash;
    string hoTen;
    string soDienThoai;
    string diaChi;

    UserRecord()
        : loai("CUSTOMER"), id(""), username(""), passwordHash(""),
          hoTen(""), soDienThoai(""), diaChi("") {}

    UserRecord(const string& loai, const string& id, const string& username,
               const string& passwordHash, const string& hoTen = "",
               const string& sdt = "", const string& diaChi = "")
        : loai(loai), id(id), username(username), passwordHash(passwordHash),
          hoTen(hoTen), soDienThoai(sdt), diaChi(diaChi) {}

    bool isAdmin() const { return loai == "ADMIN"; }
    bool isCustomer() const { return loai == "CUSTOMER"; }

    string xuatChuoiFile() const {
        if (loai == "ADMIN") {
            return "ADMIN|" + id + "|" + username + "|" + passwordHash;
        } else {
            return "CUSTOMER|" + id + "|" + username + "|" + passwordHash + "|" +
                   hoTen + "|" + soDienThoai + "|" + diaChi;
        }
    }
};

class FileManager {
private:
    string dataDir;

    // Đường dẫn đầy đủ của file
    string layDuongDan(const string& tenFile) const;

    // Tách chuỗi theo dấu phân cách | (loại bỏ khoảng trắng thừa)
    static vector<string> tachChuoi(const string& dong, char kyTuPhanCach = '|');
    static string catKhoangTrang(const string& s);

    // Ghi an toàn bằng cách ghi vào file tạm .tmp rồi đổi tên (Mục 4 README)
    bool ghiFileAnToan(const string& tenFile, const vector<string>& cacDong);

    // Thêm một dòng vào cuối file
    bool themDongVaoFile(const string& tenFile, const string& dong);

public:
    explicit FileManager(const string& duongDanThuMuc = "data");
    ~FileManager() = default;

    // Getter & Setter đường dẫn thư mục dữ liệu
    const string& getDataDir() const;
    void setDataDir(const string& duongDanThuMuc);

    // ================= 1. ĐỌC DỮ LIỆU TỪ 7 FILE =================
    // products.txt
    vector<SanPham*> docSanPham();

    // warehouses.txt
    vector<KhoHang> docKhoHang();

    // orders.txt
    vector<DonHang> docDonHang();

    // chitietdonhang.txt
    vector<ChiTietDonHang> docChiTietDonHang();

    // suppliers.txt
    vector<NhaCungCapRecord> docNhaCungCap();

    // users.txt (Trả về danh sách con trỏ đa hình TaiKhoan* gồm Admin* và NguoiDung*)
    vector<TaiKhoan*> docTaiKhoan();
    vector<TaiKhoan*> docNguoiDung() { return docTaiKhoan(); }

    // revenues.txt
    vector<DoanhThuRecord> docDoanhThu();

    // ================= 2. GHI DỮ LIỆU VÀO 7 FILE (AN TOÀN QUA FILE TẠM) =================
    bool ghiSanPham(const vector<SanPham*>& danhSach);
    bool ghiKhoHang(const vector<KhoHang>& danhSach);
    bool ghiDonHang(const vector<DonHang>& danhSach);
    bool ghiChiTietDonHang(const vector<ChiTietDonHang>& danhSach);
    bool ghiNhaCungCap(const vector<NhaCungCapRecord>& danhSach);
    bool ghiTaiKhoan(const vector<TaiKhoan*>& danhSach);
    bool ghiNguoiDung(const vector<TaiKhoan*>& danhSach) { return ghiTaiKhoan(danhSach); }
    bool ghiDoanhThu(const vector<DoanhThuRecord>& danhSach);

    // ================= 3. THÊM BẢN GHI MỚI VÀO CUỐI FILE (APPEND) =================
    bool themSanPham(const SanPham* sp);
    bool themKhoHang(const KhoHang& kho);
    bool themDonHang(const DonHang& dh);
    bool themChiTietDonHang(const ChiTietDonHang& ctdh);
    bool themNhaCungCap(const NhaCungCapRecord& ncc);
    bool themTaiKhoan(const TaiKhoan* tk);
    bool themNguoiDung(const TaiKhoan* tk) { return themTaiKhoan(tk); }
    bool themDoanhThu(const DoanhThuRecord& dt);

    // ================= 4. LIÊN KẾT ĐƠN HÀNG VÀO TÀI KHOẢN KHÁCH HÀNG (Mục 5.1 README) =================
    static void lienKetDonHangVaoNguoiDung(const vector<TaiKhoan*>& danhSachTaiKhoan, vector<DonHang>& danhSachDonHang);
    static void lienKetDonHangVaoNguoiDung(const vector<TaiKhoan*>& danhSachTaiKhoan, const vector<DonHang*>& danhSachDonHang);
};

#endif // FILEMANAGER_H
