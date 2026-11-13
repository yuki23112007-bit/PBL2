#include "QuanLyKhoNCC.h"

using namespace std;

QuanLyKhoNCC::~QuanLyKhoNCC() {
    clear();
}

void QuanLyKhoNCC::clear() {
    for (size_t i = 0; i < danhSachKho.size(); i++) delete danhSachKho[i];
    for (size_t i = 0; i < danhSachNCC.size(); i++) delete danhSachNCC[i];
    danhSachKho.clear();
    danhSachNCC.clear();
}

void QuanLyKhoNCC::napKho(const vector<KhoHang>& ds) {
    for (size_t i = 0; i < danhSachKho.size(); i++) delete danhSachKho[i];
    danhSachKho.clear();
    for (const KhoHang& k : ds) {
        danhSachKho.push_back(new KhoHang(k));
    }
}

void QuanLyKhoNCC::napNCC(const vector<NhaCungCap>& ds) {
    for (size_t i = 0; i < danhSachNCC.size(); i++) delete danhSachNCC[i];
    danhSachNCC.clear();
    for (const NhaCungCap& n : ds) {
        danhSachNCC.push_back(new NhaCungCap(n));
    }
}

bool QuanLyKhoNCC::themKho(KhoHang* kho) {
    if (kho == nullptr || timKhoTheoMa(kho->getMaKho()) != nullptr) return false;
    danhSachKho.push_back(kho);
    return true;
}

KhoHang* QuanLyKhoNCC::timKhoTheoMa(const string& maKho) const {
    for (size_t i = 0; i < danhSachKho.size(); i++) {
        if (danhSachKho[i]->getMaKho() == maKho) return danhSachKho[i];
    }
    return nullptr;
}

bool QuanLyKhoNCC::luuHoacCapNhatKho(const string& maKho, const string& viTri, int sucChua, const string& maSP) {
    KhoHang* k = timKhoTheoMa(maKho);
    if (k) {
        k->capNhatThongTin(viTri, sucChua, maSP);
        return true;
    }
    danhSachKho.push_back(new KhoHang(maKho, viTri, sucChua, maSP));
    return true;
}

bool QuanLyKhoNCC::xoaKho(const string& maKho) {
    for (size_t i = 0; i < danhSachKho.size(); i++) {
        if (danhSachKho[i]->getMaKho() == maKho) {
            delete danhSachKho[i];
            danhSachKho.erase(danhSachKho.begin() + i);
            return true;
        }
    }
    return false;
}

vector<KhoHang> QuanLyKhoNCC::xuatDanhSachKho() const {
    vector<KhoHang> out;
    out.reserve(danhSachKho.size());
    for (const KhoHang* k : danhSachKho) {
        if (k) out.push_back(*k);
    }
    return out;
}

const vector<KhoHang*>& QuanLyKhoNCC::getDanhSachKho() const {
    return danhSachKho;
}

bool QuanLyKhoNCC::themNhaCungCap(NhaCungCap* ncc) {
    if (ncc == nullptr || timNCCTheoMa(ncc->getMaNhaCungCap()) != nullptr) return false;
    danhSachNCC.push_back(ncc);
    return true;
}

NhaCungCap* QuanLyKhoNCC::timNCCTheoMa(const string& maNCC) const {
    for (size_t i = 0; i < danhSachNCC.size(); i++) {
        if (danhSachNCC[i]->getMaNhaCungCap() == maNCC) return danhSachNCC[i];
    }
    return nullptr;
}

bool QuanLyKhoNCC::luuHoacCapNhatNCC(const string& maNCC, const string& tenCT, const string& diaChi, const string& sdt, const string& danhMuc) {
    NhaCungCap* n = timNCCTheoMa(maNCC);
    if (n) {
        n->capNhatThongTin(tenCT, diaChi, sdt, danhMuc);
        return true;
    }
    danhSachNCC.push_back(new NhaCungCap(maNCC, tenCT, diaChi, sdt, danhMuc));
    return true;
}

bool QuanLyKhoNCC::xoaNCC(const string& maNCC) {
    for (size_t i = 0; i < danhSachNCC.size(); i++) {
        if (danhSachNCC[i]->getMaNhaCungCap() == maNCC) {
            delete danhSachNCC[i];
            danhSachNCC.erase(danhSachNCC.begin() + i);
            return true;
        }
    }
    return false;
}

vector<NhaCungCap> QuanLyKhoNCC::xuatDanhSachNCC() const {
    vector<NhaCungCap> out;
    out.reserve(danhSachNCC.size());
    for (const NhaCungCap* n : danhSachNCC) {
        if (n) out.push_back(*n);
    }
    return out;
}

const vector<NhaCungCap*>& QuanLyKhoNCC::getDanhSachNCC() const {
    return danhSachNCC;
}

int QuanLyKhoNCC::soLuongKho() const {
    return (int)danhSachKho.size();
}

int QuanLyKhoNCC::soLuongNCC() const {
    return (int)danhSachNCC.size();
}

