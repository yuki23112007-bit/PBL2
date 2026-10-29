#include "FileManager.h"

#include <cstdio>
#include <filesystem>
#include <system_error>

using namespace std;

namespace {

// Chuyen time_t sang chuoi "yyyy-mm-dd HH:MM:SS" (dinh dang QuanLyDonHang dang dung trong bo nho)
string thoiGianThanhChuoi(time_t t) {
    char buf[30];
    tm* thoiGian = localtime(&t);
    if (thoiGian == nullptr) {
        return "";
    }
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", thoiGian);
    return string(buf);
}

// Dong orders.txt dung 7 truong theo muc 3.5 README:
// maDonHang|tenKhachHang|diaChiKhachHang|soDienThoaiKhachHang|trangThaiDonHang|ngayDatHang|IdNguoiDung
// ngayDatHang luu duoi dang time_t (lay tu thoiGianTao cua DonHang).
// Khong dung DonHang::xuatChuoiFile() vi ham do xuat 8 truong.
string chuoiDonHang(const DonHang& dh) {
    return dh.getMaDonHang() + "|" + dh.getTenKhachHang() + "|" + dh.getDiaChiKhachHang() + "|" +
           dh.getSoDienThoaiKhachHang() + "|" + dh.getTrangThaiDonHang() + "|" +
           to_string(static_cast<long long>(dh.getThoiGianTao())) + "|" + dh.getMaNguoiDung();
}

} // namespace

// Constructor
FileManager::FileManager(const string& duongDanThuMuc)
    : dataDir(duongDanThuMuc.empty() ? "data" : duongDanThuMuc) {}

const string& FileManager::getDataDir() const {
    return dataDir;
}

void FileManager::setDataDir(const string& duongDanThuMuc) {
    dataDir = duongDanThuMuc.empty() ? "data" : duongDanThuMuc;
}

string FileManager::layDuongDan(const string& tenFile) const {
    if (dataDir.empty()) {
        return tenFile;
    }
    char kyTuCuoi = dataDir.back();
    if (kyTuCuoi == '/' || kyTuCuoi == '\\') {
        return dataDir + tenFile;
    }
    return dataDir + "/" + tenFile;
}

string FileManager::catKhoangTrang(const string& s) {
    size_t start = 0;
    while (start < s.length() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n')) {
        start++;
    }
    if (start == s.length()) {
        return "";
    }
    size_t end = s.length() - 1;
    while (end > start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n')) {
        end--;
    }
    return s.substr(start, end - start + 1);
}

vector<string> FileManager::tachChuoi(const string& dong, char kyTuPhanCach) {
    vector<string> ketQua;
    string dongDaCat = catKhoangTrang(dong);
    if (dongDaCat.empty()) {
        return ketQua;
    }

    stringstream ss(dongDaCat);
    string token;
    while (getline(ss, token, kyTuPhanCach)) {
        ketQua.push_back(catKhoangTrang(token));
    }
    if (!dongDaCat.empty() && dongDaCat.back() == kyTuPhanCach) {
        ketQua.push_back("");
    }
    return ketQua;
}

bool FileManager::ghiFileAnToan(const string& tenFile, const vector<string>& cacDong) {
    string duongDanChinh = layDuongDan(tenFile);
    string duongDanTam = duongDanChinh + ".tmp";

    ofstream fileTam(duongDanTam, ios::out | ios::trunc);
    if (!fileTam.is_open()) {
        cerr << "[FileManager] Khong the tao file tam: " << duongDanTam << "\n";
        return false;
    }

    for (const auto& dong : cacDong) {
        fileTam << dong << "\n";
    }
    fileTam.flush();
    bool ghiLoi = fileTam.fail();
    fileTam.close();

    // Ghi file tam bi loi giua chung: giu nguyen file chinh, xoa file tam
    if (ghiLoi || fileTam.fail()) {
        cerr << "[FileManager] Loi khi ghi file tam: " << duongDanTam << "\n";
        remove(duongDanTam.c_str());
        return false;
    }

    // Doi ten bang MOT thao tac (muc 4 README): std::filesystem::rename thay the file dich neu da ton tai,
    // nen file chinh luon la ban cu hoan chinh hoac ban moi hoan chinh.
    error_code ec;
    filesystem::rename(duongDanTam, duongDanChinh, ec);
    if (ec) {
        cerr << "[FileManager] Khong the doi ten file tam thanh: " << duongDanChinh
             << " (" << ec.message() << ")\n";
        remove(duongDanTam.c_str());
        return false;
    }
    return true;
}

bool FileManager::themDongVaoFile(const string& tenFile, const string& dong) {
    string duongDan = layDuongDan(tenFile);
    ofstream file(duongDan, ios::out | ios::app);
    if (!file.is_open()) {
        cerr << "[FileManager] Khong the mo file de ghi them: " << duongDan << "\n";
        return false;
    }
    file << dong << "\n";
    file.close();
    return true;
}

// =========================================================================
// 1. CÁC HÀM ĐỌC DỮ LIỆU TỪ 7 FILE
// =========================================================================

vector<SanPham*> FileManager::docSanPham() {
    vector<SanPham*> danhSach;
    string duongDan = layDuongDan("products.txt");
    ifstream file(duongDan);
    if (!file.is_open()) {
        cerr << "[FileManager] Khong the mo file: " << duongDan << "\n";
        return danhSach;
    }

    string dong;
    while (getline(file, dong)) {
        vector<string> tokens = tachChuoi(dong);
        if (tokens.size() < 8) {
            continue;
        }

        try {
            string loai = tokens[0];
            string maSP = tokens[1];
            string tenSP = tokens[2];
            string danhMuc = tokens[3];
            double donGia = stod(tokens[4]);
            int soLuongTon = stoi(tokens[5]);
            string maNCC = tokens[6];

            if (loai == "THUONG" && tokens.size() >= 10) {
                string quyCach = tokens[7];
                time_t ngaySX = static_cast<time_t>(stoll(tokens[8]));
                int hanDung = stoi(tokens[9]);
                danhSach.push_back(new SanPhamThuong(maSP, tenSP, danhMuc, donGia, soLuongTon, maNCC, ngaySX, hanDung, quyCach));
            } else if (loai == "TUOI_SONG" && tokens.size() >= 10) {
                double nhietDo = stod(tokens[7]);
                time_t ngaySX = static_cast<time_t>(stoll(tokens[8]));
                int hanDung = stoi(tokens[9]);
                danhSach.push_back(new SanPhamTuoiSong(maSP, tenSP, danhMuc, donGia, soLuongTon, maNCC, ngaySX, hanDung, nhietDo));
            } else if (loai == "CONG_NGHE") {
                time_t ngaySX = static_cast<time_t>(stoll(tokens[7]));
                danhSach.push_back(new CongNghe(maSP, tenSP, danhMuc, donGia, soLuongTon, maNCC, ngaySX));
            } else if (loai == "DO_GIA_DUNG") {
                time_t ngaySX = static_cast<time_t>(stoll(tokens[7]));
                danhSach.push_back(new DoGiaDung(maSP, tenSP, danhMuc, donGia, soLuongTon, maNCC, ngaySX));
            }
        } catch (const exception& e) {
            cerr << "[FileManager] Loi phan tich dong san pham: " << dong << " (" << e.what() << ")\n";
        }
    }
    file.close();
    return danhSach;
}

vector<KhoHang> FileManager::docKhoHang() {
    vector<KhoHang> danhSach;
    string duongDan = layDuongDan("warehouses.txt");
    ifstream file(duongDan);
    if (!file.is_open()) {
        cerr << "[FileManager] Khong the mo file: " << duongDan << "\n";
        return danhSach;
    }

    string dong;
    while (getline(file, dong)) {
        vector<string> tokens = tachChuoi(dong);
        if (tokens.size() < 4) {
            continue;
        }

        try {
            string maKho = tokens[0];
            string viTri = tokens[1];
            int sucChua = stoi(tokens[2]);
            string maSP = tokens[3];
            danhSach.push_back(KhoHang(maKho, viTri, sucChua, maSP));
        } catch (const exception& e) {
            cerr << "[FileManager] Loi phan tich kho hang: " << dong << "\n";
        }
    }
    file.close();
    return danhSach;
}

vector<DonHang> FileManager::docDonHang() {
    vector<DonHang> danhSach;
    string duongDan = layDuongDan("orders.txt");
    ifstream file(duongDan);
    if (!file.is_open()) {
        cerr << "[FileManager] Khong the mo file: " << duongDan << "\n";
        return danhSach;
    }

    string dong;
    while (getline(file, dong)) {
        vector<string> tokens = tachChuoi(dong);
        if (tokens.size() < 7) {
            continue;
        }

        try {
            string maDH = tokens[0];
            string tenKH = tokens[1];
            string diaChi = tokens[2];
            string sdt = tokens[3];
            string trangThai = tokens[4];
            string maND = tokens[6];

            // ngayDatHang trong file la time_t (muc 3.5 README) -> dung lam thoiGianTao cua don,
            // de don cu khong bi coi la vua tao luc khoi dong (sai logic huy don qua 5 phut).
            time_t thoiGianTao = static_cast<time_t>(stoll(tokens[5]));
            string ngayDat = thoiGianThanhChuoi(thoiGianTao);

            danhSach.push_back(DonHang(maDH, tenKH, diaChi, sdt, trangThai, ngayDat, maND, thoiGianTao));
        } catch (const exception& e) {
            cerr << "[FileManager] Loi phan tich don hang: " << dong << "\n";
        }
    }
    file.close();
    return danhSach;
}

vector<ChiTietDonHang> FileManager::docChiTietDonHang() {
    vector<ChiTietDonHang> danhSach;
    string duongDan = layDuongDan("chitietdonhang.txt");
    ifstream file(duongDan);
    if (!file.is_open()) {
        cerr << "[FileManager] Khong the mo file: " << duongDan << "\n";
        return danhSach;
    }

    string dong;
    while (getline(file, dong)) {
        vector<string> tokens = tachChuoi(dong);
        if (tokens.size() < 5) {
            continue;
        }

        try {
            string maCT = tokens[0];
            string maDH = tokens[1];
            string maSP = tokens[2];
            int soLuong = stoi(tokens[3]);
            double thanhTien = stod(tokens[4]);

            ChiTietDonHang ct(maCT, maDH, maSP, soLuong, 0.0);
            ct.setThanhTien(thanhTien);
            danhSach.push_back(ct);
        } catch (const exception& e) {
            cerr << "[FileManager] Loi phan tich chi tiet don hang: " << dong << "\n";
        }
    }
    file.close();
    return danhSach;
}

vector<NhaCungCapRecord> FileManager::docNhaCungCap() {
    vector<NhaCungCapRecord> danhSach;
    string duongDan = layDuongDan("suppliers.txt");
    ifstream file(duongDan);
    if (!file.is_open()) {
        cerr << "[FileManager] Khong the mo file: " << duongDan << "\n";
        return danhSach;
    }

    string dong;
    while (getline(file, dong)) {
        vector<string> tokens = tachChuoi(dong);
        if (tokens.size() < 5) {
            continue;
        }
        danhSach.push_back(NhaCungCapRecord(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4]));
    }
    file.close();
    return danhSach;
}

vector<UserRecord> FileManager::docNguoiDung() {
    vector<UserRecord> danhSach;
    string duongDan = layDuongDan("users.txt");
    ifstream file(duongDan);
    if (!file.is_open()) {
        cerr << "[FileManager] Khong the mo file: " << duongDan << "\n";
        return danhSach;
    }

    string dong;
    while (getline(file, dong)) {
        vector<string> tokens = tachChuoi(dong);
        if (tokens.size() < 4) {
            continue;
        }

        string loai = tokens[0];
        string id = tokens[1];
        string username = tokens[2];
        string pass = tokens[3];

        if (loai == "ADMIN") {
            danhSach.push_back(UserRecord("ADMIN", id, username, pass));
        } else if (loai == "CUSTOMER" && tokens.size() >= 7) {
            string hoTen = tokens[4];
            string sdt = tokens[5];
            string diaChi = tokens[6];
            danhSach.push_back(UserRecord("CUSTOMER", id, username, pass, hoTen, sdt, diaChi));
        }
    }
    file.close();
    return danhSach;
}

vector<DoanhThuRecord> FileManager::docDoanhThu() {
    vector<DoanhThuRecord> danhSach;
    string duongDan = layDuongDan("revenues.txt");
    ifstream file(duongDan);
    if (!file.is_open()) {
        cerr << "[FileManager] Khong the mo file: " << duongDan << "\n";
        return danhSach;
    }

    string dong;
    while (getline(file, dong)) {
        vector<string> tokens = tachChuoi(dong);
        if (tokens.size() < 3) {
            continue;
        }

        try {
            string ngay = tokens[0];
            double tongDT = stod(tokens[1]);
            int soDon = stoi(tokens[2]);
            danhSach.push_back(DoanhThuRecord(ngay, tongDT, soDon));
        } catch (const exception& e) {
            cerr << "[FileManager] Loi phan tich doanh thu: " << dong << "\n";
        }
    }
    file.close();
    return danhSach;
}

// =========================================================================
// 2. CÁC HÀM GHI DỮ LIỆU VÀO 7 FILE (AN TOÀN QUA FILE TẠM .tmp)
// =========================================================================

bool FileManager::ghiSanPham(const vector<SanPham*>& danhSach) {
    vector<string> cacDong;
    cacDong.reserve(danhSach.size());
    for (const auto* sp : danhSach) {
        if (sp != nullptr) {
            cacDong.push_back(sp->xuatChuoiFile());
        }
    }
    return ghiFileAnToan("products.txt", cacDong);
}

bool FileManager::ghiKhoHang(const vector<KhoHang>& danhSach) {
    vector<string> cacDong;
    cacDong.reserve(danhSach.size());
    for (const auto& kho : danhSach) {
        cacDong.push_back(kho.xuatChuoiFile());
    }
    return ghiFileAnToan("warehouses.txt", cacDong);
}

bool FileManager::ghiDonHang(const vector<DonHang>& danhSach) {
    vector<string> cacDong;
    cacDong.reserve(danhSach.size());
    for (const auto& dh : danhSach) {
        cacDong.push_back(chuoiDonHang(dh));
    }
    return ghiFileAnToan("orders.txt", cacDong);
}

bool FileManager::ghiChiTietDonHang(const vector<ChiTietDonHang>& danhSach) {
    vector<string> cacDong;
    cacDong.reserve(danhSach.size());
    for (const auto& ct : danhSach) {
        cacDong.push_back(ct.xuatChuoiFile());
    }
    return ghiFileAnToan("chitietdonhang.txt", cacDong);
}

bool FileManager::ghiNhaCungCap(const vector<NhaCungCapRecord>& danhSach) {
    vector<string> cacDong;
    cacDong.reserve(danhSach.size());
    for (const auto& ncc : danhSach) {
        cacDong.push_back(ncc.xuatChuoiFile());
    }
    return ghiFileAnToan("suppliers.txt", cacDong);
}

bool FileManager::ghiNguoiDung(const vector<UserRecord>& danhSach) {
    vector<string> cacDong;
    cacDong.reserve(danhSach.size());
    for (const auto& u : danhSach) {
        cacDong.push_back(u.xuatChuoiFile());
    }
    return ghiFileAnToan("users.txt", cacDong);
}

bool FileManager::ghiDoanhThu(const vector<DoanhThuRecord>& danhSach) {
    vector<string> cacDong;
    cacDong.reserve(danhSach.size());
    for (const auto& dt : danhSach) {
        cacDong.push_back(dt.xuatChuoiFile());
    }
    return ghiFileAnToan("revenues.txt", cacDong);
}

// =========================================================================
// 3. CÁC HÀM THÊM BẢN GHI MỚI VÀO CUỐI FILE (APPEND)
// =========================================================================

bool FileManager::themSanPham(const SanPham* sp) {
    if (sp == nullptr) return false;
    return themDongVaoFile("products.txt", sp->xuatChuoiFile());
}

bool FileManager::themKhoHang(const KhoHang& kho) {
    return themDongVaoFile("warehouses.txt", kho.xuatChuoiFile());
}

bool FileManager::themDonHang(const DonHang& dh) {
    return themDongVaoFile("orders.txt", chuoiDonHang(dh));
}

bool FileManager::themChiTietDonHang(const ChiTietDonHang& ctdh) {
    return themDongVaoFile("chitietdonhang.txt", ctdh.xuatChuoiFile());
}

bool FileManager::themNhaCungCap(const NhaCungCapRecord& ncc) {
    return themDongVaoFile("suppliers.txt", ncc.xuatChuoiFile());
}

bool FileManager::themNguoiDung(const UserRecord& user) {
    return themDongVaoFile("users.txt", user.xuatChuoiFile());
}

bool FileManager::themDoanhThu(const DoanhThuRecord& dt) {
    return themDongVaoFile("revenues.txt", dt.xuatChuoiFile());
}
