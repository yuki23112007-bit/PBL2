#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <ctime>

#include "../data/FileManager.h"
#include "../service/QuanLySanPham.h"
#include "../service/QuanLyDonHang.h"
#include "../core/Trie.h"
#include "../models/TaiKhoan.h"
#include "../models/Admin.h"
#include "../models/NguoiDung.h"
#include "../models/KhoHang.h"
#include "../models/NhaCungCap.h"
#include "../models/DoanhThuNgay.h"
#include "../models/DanhGiaDonHang.h"

using namespace std;

// Phien dang nhap phia may chu: web chi gui token, may chu tu biet ai la ai.
struct PhienDangNhap {
    string idNguoiDung;
    string vaiTro;      // "ADMIN" hoac "CUSTOMER"
    time_t hetHan;
};

class WebServer {
private:
    int port;
    string webRoot;
    string dataDir;
    atomic<bool> isRunning;
    mutex dataMutex; // Cac luong xu ly request dung chung du lieu nghiep vu

    // Du lieu nghiep vu cot loi cua PBL 2
    FileManager fileManager;
    QuanLySanPham qlSanPham;
    QuanLyDonHang qlDonHang;
    Trie cayTrieTaiKhoan;                        // Cay tien to Trie luu tru username phuc vu tra cuu va chan trung lap O(L)
    vector<TaiKhoan*> danhSachTaiKhoan;
    vector<KhoHang> danhSachKho;
    vector<NhaCungCap> danhSachNCC;
    vector<DoanhThuNgay> danhSachDoanhThu;

    // Du lieu bo sung cua lop web (file moi trong thu muc du lieu, khong dong vao file cu)
    vector<DanhGiaDonHang> danhSachDanhGia;      // danhgia.txt
    map<string, time_t> thoiGianGiaoHang;        // giaohang.txt: ma don -> luc chuyen sang "Dang giao"
    map<string, PhienDangNhap> phienDangNhap;    // token -> phien (chi giu trong bo nho)

    // Khoi tao va nap du lieu tu cac file trong data/
    void napDuLieuHeThong();
    void luuDuLieuHeThong();
    void napDanhGia();
    void luuDanhGia();
    void napGiaoHang();
    void luuGiaoHang();

    // Xu ly Request HTTP & Tra Response
    void handleClient(uintptr_t clientSocket);
    void routeRequest(const string& method, const string& path, const string& query,
                      const string& body, const map<string, string>& headers,
                      int& status, string& responseBody, string& contentType);
    bool phucVuFileTinh(const string& path, int& status, string& responseBody, string& contentType);

    // Helpers cho JSON & Parsing
    static string escapeJson(const string& s);
    static string urlDecode(const string& in);
    static map<string, string> parseQueryParams(const string& query);
    static map<string, string> parseSimpleJson(const string& json);
    static string getMimeType(const string& path);
    static string loiJson(const string& message);
    static string lamSachTruongFile(const string& s);

    // Xac thuc & phan quyen
    bool xacThuc(const map<string, string>& headers, PhienDangNhap& phien);
    string taoToken();
    TaiKhoan* timTaiKhoanTheoId(const string& id) const;

    // Trang thai don hang: ma chuan <-> chuoi noi bo cua QuanLyDonHang <-> nhan hien thi
    static string maTrangThai(const string& s);
    static string chuoiNoiBo(const string& ma);
    static string nhanTrangThai(const string& ma);
    static bool laTrangThaiKetThuc(const string& ma);
    static bool daQuaHan3Ngay(time_t moc, time_t bayGio);
    static string ngayGioHienThi(time_t t);
    static string ngayIso(time_t t);

    // Nghiep vu bo sung tren don hang
    time_t mocTinhHan(const DonHang* dh) const;
    time_t thoiGianGiaoDuKien(const DonHang* dh) const;
    time_t hanChotPhanHoi(const DonHang* dh) const;
    bool daQuaHan3NgaySoVoiGiaoDuKien(const DonHang* dh, time_t bayGio) const;
    const DanhGiaDonHang* timDanhGia(const string& maDH) const;
    bool coKhieuNaiDangMo(const string& maDH) const;
    void ghiNhanDoanhThu(const DonHang* dh);
    void kiemTraDonQuaHan();
    double tongTienDon(const string& maDH) const;
    string donHangJson(const DonHang* dh, bool kemChiTiet) const;
    string sanPhamJson(SanPham* sp) const;
    void thuHoiDonPending();

    // Cac API Endpoints (khach)
    string apiLogin(const string& body);
    string apiRegister(const string& body);
    string apiCheckUsername(const map<string, string>& params);
    string apiLogout(const map<string, string>& headers);
    string apiGetProducts(const map<string, string>& params);
    string apiSearchProducts(const map<string, string>& params);
    string apiGetProductDetail(const map<string, string>& params);
    string apiCreateOrder(const string& body, const PhienDangNhap& phien);
    string apiGetOrders(const PhienDangNhap& phien);
    string apiLookupOrder(const map<string, string>& params, const PhienDangNhap& phien, int& status);
    string apiCancelOrder(const string& body, const PhienDangNhap& phien);
    string apiConfirmReceived(const string& body, const PhienDangNhap& phien);
    string apiComplainOrder(const string& body, const PhienDangNhap& phien);
    string apiReviewOrder(const string& body, const PhienDangNhap& phien);

    // Cac API Endpoints (admin)
    string apiGetOrderQueue();
    string apiGetPendingOrders();
    string apiGetOverdueOrders();
    string apiApproveOrder();
    string apiShipOrder(const string& body);
    string apiGetWarehouseAlerts();
    string apiGetWarehouses();
    string apiSaveWarehouse(const string& body);
    string apiDeleteWarehouse(const string& body);
    string apiGetSuppliers();
    string apiSaveSupplier(const string& body);
    string apiDeleteSupplier(const string& body);
    string apiGetRevenues(const map<string, string>& params);
    string apiAdminGetProducts(const map<string, string>& params);
    string apiAddProduct(const string& body);
    string apiUpdateProduct(const string& body);
    string apiDeleteProduct(const string& body);
    string apiUpdateStock(const string& body);
    string apiGetUsers();
    string apiSaveUser(const string& body, const PhienDangNhap& phien);
    string apiDeleteUser(const string& body, const PhienDangNhap& phien);
    string apiResetPassword(const string& body);
    string apiGetReviews();

public:
    explicit WebServer(int port = 8080, const string& webRoot = "web", const string& dataDir = "data");
    ~WebServer();

    WebServer(const WebServer&) = delete;
    WebServer& operator=(const WebServer&) = delete;

    void start();
    void stop();
};

#endif // WEBSERVER_H
