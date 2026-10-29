#include "VanChuyen.h"
#include <iomanip>

VanChuyen::VanChuyen()
    : maVanChuyen(""), maDonHang(""), ngayTao(time(nullptr)),
      trangThaiGiao("Dang Giao"), chiPhiVanChuyen(0.0), loaiXe(""),
      thoiGianGiaoDuKien(ngayTao + 2 * 86400) {}

VanChuyen::VanChuyen(const std::string& maVC, const std::string& maDH,
                     time_t tao, const std::string& trangThai,
                     double chiPhi, const std::string& xe,
                     time_t duKien)
    : maVanChuyen(maVC), maDonHang(maDH), ngayTao(tao),
      trangThaiGiao(trangThai), chiPhiVanChuyen(chiPhi), loaiXe(xe),
      thoiGianGiaoDuKien(duKien == 0 ? tao + 2 * 86400 : duKien) {}

const std::string& VanChuyen::getMaVanChuyen() const {
    return maVanChuyen;
}

const std::string& VanChuyen::getMaDonHang() const {
    return maDonHang;
}

time_t VanChuyen::getNgayTao() const {
    return ngayTao;
}

const std::string& VanChuyen::getTrangThaiGiao() const {
    return trangThaiGiao;
}

double VanChuyen::getChiPhiVanChuyen() const {
    return chiPhiVanChuyen;
}

const std::string& VanChuyen::getLoaiXe() const {
    return loaiXe;
}

time_t VanChuyen::getThoiGianGiaoDuKien() const {
    return thoiGianGiaoDuKien;
}

void VanChuyen::setMaVanChuyen(const std::string& maVC) {
    maVanChuyen = maVC;
}

void VanChuyen::setMaDonHang(const std::string& maDH) {
    maDonHang = maDH;
}

void VanChuyen::setNgayTao(time_t tao) {
    ngayTao = tao;
}

void VanChuyen::setTrangThaiGiao(const std::string& trangThai) {
    trangThaiGiao = trangThai;
}

void VanChuyen::setChiPhiVanChuyen(double chiPhi) {
    chiPhiVanChuyen = chiPhi;
}

void VanChuyen::setLoaiXe(const std::string& xe) {
    loaiXe = xe;
}

void VanChuyen::setThoiGianGiaoDuKien(time_t duKien) {
    thoiGianGiaoDuKien = duKien;
}

void VanChuyen::capNhatTrangThaiGiao(const std::string& trangThaiMoi) {
    trangThaiGiao = trangThaiMoi;
}

bool VanChuyen::daQuaHan3Ngay(time_t thoiGianHienTai) const {
    if (thoiGianGiaoDuKien == 0) return false;
    return thoiGianHienTai > (thoiGianGiaoDuKien + 3 * 86400);
}

double VanChuyen::tinhChiPhiVanChuyen() const {
    return chiPhiVanChuyen;
}

void VanChuyen::hienThiThongTin() const {
    std::cout << "[VAN DON: " << maVanChuyen << "]\n";
    std::cout << "  - Ma don hang     : " << maDonHang << "\n";
    std::cout << "  - Ngay tao        : " << ngayTao << "\n";
    std::cout << "  - Loai xe         : " << loaiXe << "\n";
    std::cout << "  - Phi ship        : " << std::fixed << std::setprecision(0) << chiPhiVanChuyen << " VND\n";
    std::cout << "  - Trang thai      : " << trangThaiGiao << "\n";
    std::cout << "  - Giao du kien    : " << thoiGianGiaoDuKien << "\n";
}

std::string VanChuyen::xuatChuoiFile() const {
    return maVanChuyen + "|" + maDonHang + "|" + std::to_string(ngayTao) + "|" +
           trangThaiGiao + "|" + std::to_string(static_cast<long long>(chiPhiVanChuyen)) + "|" +
           loaiXe + "|" + std::to_string(thoiGianGiaoDuKien);
}

std::ostream& operator<<(std::ostream& os, const VanChuyen& vc) {
    os << vc.xuatChuoiFile();
    return os;
}

std::istream& operator>>(std::istream& is, VanChuyen& vc) {
    is >> vc.maVanChuyen >> vc.maDonHang >> vc.ngayTao >> vc.trangThaiGiao
       >> vc.chiPhiVanChuyen >> vc.loaiXe >> vc.thoiGianGiaoDuKien;
    return is;
}
