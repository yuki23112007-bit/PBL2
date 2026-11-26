#include "VanChuyen.h"
#include "PhuongTien.h"

using namespace std;

VanChuyen::VanChuyen()
    : maVanChuyen(""), maDonHang(""), ngayTao(time(nullptr)),
      trangThaiGiao("Dang Giao"), chiPhiVanChuyen(25000.0), loaiXe("Xe May Giao Nhanh"),
      thoiGianGiaoDuKien(ngayTao + 2 * 86400), phuongTien(nullptr) {}

VanChuyen::VanChuyen(const string& maVC, const string& maDH,
                     time_t tao, const string& trangThai,
                     double chiPhi, const string& xe,
                     time_t duKien, PhuongTien* pt)
    : maVanChuyen(maVC), maDonHang(maDH), ngayTao(tao),
      trangThaiGiao(trangThai), chiPhiVanChuyen(chiPhi), loaiXe(xe),
      thoiGianGiaoDuKien(duKien == 0 ? tao + 2 * 86400 : duKien), phuongTien(pt) {}

const string& VanChuyen::getMaVanChuyen() const {
    return maVanChuyen;
}

const string& VanChuyen::getMaDonHang() const {
    return maDonHang;
}

time_t VanChuyen::getNgayTao() const {
    return ngayTao;
}

const string& VanChuyen::getTrangThaiGiao() const {
    return trangThaiGiao;
}

double VanChuyen::getChiPhiVanChuyen() const {
    return chiPhiVanChuyen;
}

const string& VanChuyen::getLoaiXe() const {
    return loaiXe;
}

time_t VanChuyen::getThoiGianGiaoDuKien() const {
    return thoiGianGiaoDuKien;
}

PhuongTien* VanChuyen::getPhuongTien() const {
    return phuongTien;
}

void VanChuyen::setNgayTao(time_t tao) {
    ngayTao = tao;
}

void VanChuyen::setTrangThaiGiao(const string& trangThai) {
    trangThaiGiao = trangThai;
}

void VanChuyen::setChiPhiVanChuyen(double chiPhi) {
    chiPhiVanChuyen = chiPhi;
}

void VanChuyen::setLoaiXe(const string& xe) {
    loaiXe = xe;
}

void VanChuyen::setThoiGianGiaoDuKien(time_t duKien) {
    thoiGianGiaoDuKien = duKien;
}

void VanChuyen::ganPhuongTien(PhuongTien* pt) {
    phuongTien = pt;
    if (phuongTien != nullptr) {
        loaiXe = phuongTien->getLoaiPhuongTien();
    }
}

void VanChuyen::capNhatTrangThaiGiao(const string& trangThaiMoi) {
    trangThaiGiao = trangThaiMoi;
}

bool VanChuyen::daQuaHan3Ngay(time_t thoiGianHienTai) const {
    if (thoiGianGiaoDuKien == 0) return false;
    return thoiGianHienTai > (thoiGianGiaoDuKien + 3 * 86400);
}

double VanChuyen::tinhChiPhiVanChuyen(double quangDuongKm) {
    if (phuongTien != nullptr) {
        chiPhiVanChuyen = phuongTien->tinhChiPhiVanChuyen(quangDuongKm);
    }
    return chiPhiVanChuyen;
}
