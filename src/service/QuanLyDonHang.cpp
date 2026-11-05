#include "QuanLyDonHang.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

string QuanLyDonHang::taoMaDonHangMoi() {
    demDonHang++;
    ostringstream oss;
    oss << "DH" << setw(3) << setfill('0') << demDonHang;
    return oss.str();
}

string QuanLyDonHang::taoMaChiTietMoi() {
    demChiTiet++;
    ostringstream oss;
    oss << "CT" << setw(3) << setfill('0') << demChiTiet;
    return oss.str();
}

QuanLyDonHang::QuanLyDonHang()
    : demDonHang(0), demChiTiet(0) {}

QuanLyDonHang::~QuanLyDonHang() {
    for (DonHang* dh : danhSachDonHang) {
        delete dh;
    }
    danhSachDonHang.clear();
}

bool QuanLyDonHang::datHang(const string& maND, QuanLySanPham& qlSP,
                            const string& maSP, int soLuong,
                            const string& tenKH, const string& diaChi,
                            const string& sdt, string& maDHMoi) {
    if (soLuong <= 0) {
        return false;
    }

    SanPham* sp = qlSP.timSanPhamTheoMa(maSP);
    if (sp == nullptr) {
        return false;
    }

    // Sinh ma don va ma chi tiet
    string maDH = taoMaDonHangMoi();
    string maCT = taoMaChiTietMoi();
    maDHMoi = maDH;

    time_t now = time(nullptr);
    char buf[30];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    string ngayDat(buf);

    // Tao ChiTietDonHang
    ChiTietDonHang ctdh(maCT, maDH, maSP, soLuong, sp->getDonGia());
    danhSachCTDH.push_back(ctdh);

    // Tao DonHang voi trang thai "Cho Duyet"
    DonHang* dh = new DonHang(maDH, tenKH, diaChi, sdt, "Cho Duyet", ngayDat, maND, now);
    danhSachDonHang.push_back(dh);

    // Dua vao Cay nhi phan BST va Hang doi FIFO OrderQueue
    cayDonHang.chen(maDH, dh);
    hangDoiDonHang.enqueue(dh);

    return true;
}

bool QuanLyDonHang::duyetDonHangTiepTheo(QuanLySanPham& qlSP, string& thongBao) {
    DonHang* dh = nullptr;
    if (!hangDoiDonHang.peek(dh) || dh == nullptr) {
        thongBao = "Hang doi rong, khong co don hang can duyet.";
        return false;
    }

    // Neu don da bi huy truoc do khi con trong hang doi
    if (dh->getTrangThaiDonHang() == "Da Huy" || dh->getTrangThaiDonHang() == "Da huy do qua han") {
        hangDoiDonHang.dequeue(dh);
        thongBao = "Bo qua don hang da bi huy: " + dh->getMaDonHang();
        return true;
    }

    // Kiem tra ton kho cho tat ca cac san pham trong don
    bool duTonKho = true;
    vector<ChiTietDonHang*> cacCTDH;
    for (ChiTietDonHang& ct : danhSachCTDH) {
        if (ct.getMaDonHang() == dh->getMaDonHang()) {
            cacCTDH.push_back(&ct);
            SanPham* sp = qlSP.timSanPhamTheoMa(ct.getMaSanPham());
            if (sp == nullptr || sp->getSoLuongTonKho() < ct.getSoLuong()) {
                duTonKho = false;
            }
        }
    }

    if (duTonKho) {
        // Du ton kho: Dequeue khoi Main Queue, tru kho va chuyen trang thai sang "Da Duyet"
        hangDoiDonHang.dequeue(dh);
        for (ChiTietDonHang* ct : cacCTDH) {
            qlSP.capNhatSoLuongKho(ct->getMaSanPham(), -ct->getSoLuong());
        }
        dh->setTrangThaiDonHang("Da Duyet");
        thongBao = "Duyet thanh cong don hang " + dh->getMaDonHang() + " (Da tru ton kho va san sang giao).";
        return true;
    } else {
        // Thieu ton kho: Chuyen sang Pending List de tranh nghen Head-of-Line Blocking
        hangDoiDonHang.tachSangDanhSachCho(dh->getMaDonHang());
        thongBao = "Don hang " + dh->getMaDonHang() + " thieu ton kho, da chuyen sang danh sach treo (Pending List).";
        return false;
    }
}

void QuanLyDonHang::kiemTraVaHuyDonTimeout() {
    time_t now = time(nullptr);
    vector<DonHang*> biHuy = hangDoiDonHang.kiemTraVaHuyDonQuaHan5Phut(now);
    if (!biHuy.empty()) {
        cout << "[TIMEOUT] Da tu dong huy " << biHuy.size() << " don hang cho qua 5 phut:\n";
        for (DonHang* dh : biHuy) {
            if (dh != nullptr) {
                cout << " - Ma don: " << dh->getMaDonHang() << " (" << dh->getTrangThaiDonHang() << ")\n";
            }
        }
    }
}

bool QuanLyDonHang::huyDonHang(const string& maDH, const string& maND) {
    DonHang* dh = cayDonHang.timKiem(maDH);
    if (dh == nullptr) {
        return false;
    }

    if (!maND.empty() && dh->getMaNguoiDung() != maND) {
        return false;
    }

    // Chi cho phep huy neu don hang dang o trang thai cho duyet
    if (dh->getTrangThaiDonHang() == "Cho Duyet" || dh->getTrangThaiDonHang() == "Dang Cho Hang") {
        dh->setTrangThaiDonHang("Da Huy");
        return true;
    }

    return false;
}

void QuanLyDonHang::xemLichSuDonHang(const string& maND) const {
    cout << "===== LICH SU DON HANG CUA NGUOI DUNG: " << maND << " =====\n";
    bool coDon = false;

    for (DonHang* dh : danhSachDonHang) {
        if (dh != nullptr && (maND.empty() || dh->getMaNguoiDung() == maND)) {
            coDon = true;
            cout << "------------------------------------\n";
            dh->show();
            cout << "Chi tiet mat hang:\n";
            for (const ChiTietDonHang& ct : danhSachCTDH) {
                if (ct.getMaDonHang() == dh->getMaDonHang()) {
                    cout << "   + SP: " << ct.getMaSanPham()
                         << " | SL: " << ct.getSoLuong()
                         << " | Thanh tien: " << ct.getThanhTien() << "\n";
                }
            }
        }
    }

    if (!coDon) {
        cout << "Khong co don hang nao.\n";
    }
}

void QuanLyDonHang::xemDanhSachDonHangBST() const {
    vector<DonHang*> ds = cayDonHang.layDanhSachSapXep();
    cout << "===== DANH SACH DON HANG (DUYET BST IN-ORDER) =====\n";
    for (DonHang* dh : ds) {
        if (dh != nullptr) {
            cout << "------------------------------------\n";
            dh->show();
        }
    }
}

DonHang* QuanLyDonHang::timDonHangTheoMa(const string& maDH) const {
    return cayDonHang.timKiem(maDH);
}

void QuanLyDonHang::xemHangDoi() const {
    vector<DonHang*> ds = hangDoiDonHang.layTatCaDonTrongQueue();
    cout << "===== HANG DOI DUYET DON (MAIN QUEUE - " << ds.size() << ") =====\n";
    for (size_t i = 0; i < ds.size(); ++i) {
        cout << i + 1 << ". [" << ds[i]->getMaDonHang() << "] "
             << ds[i]->getTenKhachHang() << " - Trang thai: "
             << ds[i]->getTrangThaiDonHang() << "\n";
    }
}

void QuanLyDonHang::xemDanhSachPending() const {
    vector<DonHang*> ds = hangDoiDonHang.layTatCaDonPending();
    cout << "===== DANH SACH TREO (PENDING LIST - " << ds.size() << ") =====\n";
    for (size_t i = 0; i < ds.size(); ++i) {
        cout << i + 1 << ". [" << ds[i]->getMaDonHang() << "] "
             << ds[i]->getTenKhachHang() << " - Trang thai: "
             << ds[i]->getTrangThaiDonHang() << "\n";
    }
}

bool QuanLyDonHang::phucHoiDonPending(const string& maDH, QuanLySanPham& qlSP) {
    // Kiem tra xem kho da du hang chua
    bool duHang = true;
    for (const ChiTietDonHang& ct : danhSachCTDH) {
        if (ct.getMaDonHang() == maDH) {
            SanPham* sp = qlSP.timSanPhamTheoMa(ct.getMaSanPham());
            if (sp == nullptr || sp->getSoLuongTonKho() < ct.getSoLuong()) {
                duHang = false;
                break;
            }
        }
    }

    if (!duHang) {
        return false;
    }

    DonHang* dh = nullptr;
    if (hangDoiDonHang.phucHoiDonCho(maDH, dh) && dh != nullptr) {
        hangDoiDonHang.enqueue(dh);
        return true;
    }

    return false;
}

OrderQueue& QuanLyDonHang::getHangDoi() {
    return hangDoiDonHang;
}

BinarySearchTree& QuanLyDonHang::getCayDonHang() {
    return cayDonHang;
}

const vector<DonHang*>& QuanLyDonHang::getDanhSachDonHang() const {
    return danhSachDonHang;
}

const vector<ChiTietDonHang>& QuanLyDonHang::getDanhSachCTDH() const {
    return danhSachCTDH;
}

void QuanLyDonHang::themDonHangCoSan(DonHang* dh, const vector<ChiTietDonHang>& chiTiets) {
    if (dh == nullptr) return;
    danhSachDonHang.push_back(dh);
    cayDonHang.chen(dh->getMaDonHang(), dh);
    for (const ChiTietDonHang& ct : chiTiets) {
        danhSachCTDH.push_back(ct);
    }
    if (dh->getTrangThaiDonHang() == "Cho Duyet") {
        hangDoiDonHang.enqueue(dh);
    } else if (dh->getTrangThaiDonHang() == "Dang Cho Hang") {
        hangDoiDonHang.themVaoPending(dh, dh->getThoiGianTao());
    }
}
