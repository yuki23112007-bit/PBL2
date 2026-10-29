#include "QuanLySanPham.h"

#include <iostream>

using namespace std;

static void swapSP(SanPham*& a, SanPham*& b) {
    SanPham* t = a;
    a = b;
    b = t;
}

QuanLySanPham::QuanLySanPham(int dungLuongBangBam)
    : bangBamSanPham(dungLuongBangBam) {}

bool QuanLySanPham::themSanPham(SanPham* sp) {
    if (sp == nullptr) return false;

    bool themVaoHash = bangBamSanPham.them(sp->getMaSP(), sp);
    if (!themVaoHash) {
        return false;
    }

    // Dong bo cay Trie de tim kiem theo ten
    cayTienToSanPham.chen(sp->getTenSP(), sp->getMaSP(), sp);

    // Neu la hang tuoi song, dua vao MinHeap de quan ly han dung
    if (sp->canbaoquanlanh()) {
        heapTuoiSong.them(sp);
    }

    return true;
}

bool QuanLySanPham::capNhatSoLuongKho(const string& maSP, int soLuongThayDoi) {
    SanPham* sp = bangBamSanPham.timKiem(maSP);
    if (sp == nullptr) {
        return false;
    }

    int slMoi = sp->getSoLuongTonKho() + soLuongThayDoi;
    if (slMoi < 0) {
        return false;
    }

    return sp->CapNhatSoLuongTonKho(slMoi);
}

bool QuanLySanPham::xoaSanPham(const string& maSP) {
    SanPham* sp = bangBamSanPham.timKiem(maSP);
    if (sp == nullptr) {
        return false;
    }

    // Xoa tren Trie truoc de tranh con tro treo
    cayTienToSanPham.xoa(sp->getTenSP());

    // Neu la hang tuoi song, xoa khoi MinHeap
    if (sp->canbaoquanlanh()) {
        heapTuoiSong.xoaTheoMa(maSP);
    }

    // Xoa va giai phong vung nho tren HashTable
    return bangBamSanPham.xoa(maSP);
}

SanPham* QuanLySanPham::timSanPhamTheoMa(const string& maSP) {
    return bangBamSanPham.timKiem(maSP);
}

vector<SanPham*> QuanLySanPham::timKiemTheoTen(const string& tuKhoa) {
    return cayTienToSanPham.traCuuSanPham(tuKhoa);
}

// QuickSort sap xep theo don gia (thuat toan viet tay tu cai dat)
void QuanLySanPham::quickSortGia(vector<SanPham*>& arr, int low, int high, bool tangDan) {
    if (low >= high) return;

    double pivot = arr[high]->getDonGia();
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        bool thoaMan = tangDan ? (arr[j]->getDonGia() <= pivot)
                               : (arr[j]->getDonGia() >= pivot);
        if (thoaMan) {
            i++;
            swapSP(arr[i], arr[j]);
        }
    }
    swapSP(arr[i + 1], arr[high]);
    int p = i + 1;

    quickSortGia(arr, low, p - 1, tangDan);
    quickSortGia(arr, p + 1, high, tangDan);
}

// QuickSort sap xep theo so luong ton kho
void QuanLySanPham::quickSortTonKho(vector<SanPham*>& arr, int low, int high, bool tangDan) {
    if (low >= high) return;

    int pivot = arr[high]->getSoLuongTonKho();
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        bool thoaMan = tangDan ? (arr[j]->getSoLuongTonKho() <= pivot)
                               : (arr[j]->getSoLuongTonKho() >= pivot);
        if (thoaMan) {
            i++;
            swapSP(arr[i], arr[j]);
        }
    }
    swapSP(arr[i + 1], arr[high]);
    int p = i + 1;

    quickSortTonKho(arr, low, p - 1, tangDan);
    quickSortTonKho(arr, p + 1, high, tangDan);
}

// QuickSort sap xep theo ten san pham (A-Z)
void QuanLySanPham::quickSortTen(vector<SanPham*>& arr, int low, int high) {
    if (low >= high) return;

    string pivot = arr[high]->getTenSP();
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (arr[j]->getTenSP() <= pivot) {
            i++;
            swapSP(arr[i], arr[j]);
        }
    }
    swapSP(arr[i + 1], arr[high]);
    int p = i + 1;

    quickSortTen(arr, low, p - 1);
    quickSortTen(arr, p + 1, high);
}

vector<SanPham*> QuanLySanPham::layDanhSachSanPham(int tieuChiSapXep) {
    vector<SanPham*> ds = bangBamSanPham.layTatCaConTro();
    if (ds.empty()) return ds;

    int n = static_cast<int>(ds.size());
    switch (tieuChiSapXep) {
        case 1: // Gia tang dan
            quickSortGia(ds, 0, n - 1, true);
            break;
        case 2: // Gia giam dan
            quickSortGia(ds, 0, n - 1, false);
            break;
        case 3: // Ton kho tang dan
            quickSortTonKho(ds, 0, n - 1, true);
            break;
        case 4: // Ton kho giam dan
            quickSortTonKho(ds, 0, n - 1, false);
            break;
        case 5: // Ten A-Z
            quickSortTen(ds, 0, n - 1);
            break;
        default:
            break;
    }
    return ds;
}

void QuanLySanPham::kiemTraCanhBaoHetHang(int nguong) const {
    vector<SanPham*> tatCa = bangBamSanPham.layTatCaConTro();
    bool coCanhBao = false;

    for (SanPham* sp : tatCa) {
        if (sp != nullptr && sp->KiemTranCanhBaoHetHang(nguong)) {
            if (!coCanhBao) {
                cout << "[CANH BAO HET HANG] Danh sach san pham ton kho <= " << nguong << ":\n";
                coCanhBao = true;
            }
            cout << " - [" << sp->getMaSP() << "] " << sp->getTenSP()
                 << " (Ton kho: " << sp->getSoLuongTonKho() << ")\n";
        }
    }

    if (!coCanhBao) {
        cout << "[THONG TIN] Khong co san pham nao duoi nguong ton kho toi thieu (" << nguong << ").\n";
    }
}

vector<SanPham*> QuanLySanPham::kiemTraCanhBaoHetHan(int soNgay) const {
    return heapTuoiSong.kiemTraSapHetHan(soNgay);
}

void QuanLySanPham::hienThiToanBo() const {
    vector<SanPham*> ds = bangBamSanPham.layTatCaConTro();
    cout << "===== DANH SACH SAN PHAM (" << ds.size() << ") =====\n";
    for (SanPham* sp : ds) {
        if (sp != nullptr) {
            cout << "------------------------------------\n";
            sp->show();
            cout << "\n";
        }
    }
}

HashTable& QuanLySanPham::getBangBam() {
    return bangBamSanPham;
}

Trie& QuanLySanPham::getCayTienTo() {
    return cayTienToSanPham;
}

MinHeap& QuanLySanPham::getMinHeap() {
    return heapTuoiSong;
}
