#include "VanChuyenFormatter.h"
#include "../models/PhuongTien.h"
#include <iomanip>

using namespace std;

void VanChuyenFormatter::hienThiThongTin(const VanChuyen& vc, ostream& os) {
    os << "[VAN DON: " << vc.getMaVanChuyen() << "]\n";
    os << "  - Ma don hang     : " << vc.getMaDonHang() << "\n";
    os << "  - Trang thai giao : " << vc.getTrangThaiGiao() << "\n";
    os << "  - Loai phuong tien: " << vc.getLoaiXe() << "\n";
    os << "  - Phi ship        : " << fixed << setprecision(0) << vc.getChiPhiVanChuyen() << " VND\n";
    os << "  - Ngay tao (epoch): " << vc.getNgayTao() << "\n";
    os << "  - Giao du kien    : " << vc.getThoiGianGiaoDuKien() << "\n";
    if (vc.getPhuongTien() != nullptr) {
        os << "  - Thong tin xe    : ";
        vc.getPhuongTien()->hienThiThongTin();
    }
}

string VanChuyenFormatter::xuatChuoiFile(const VanChuyen& vc) {
    return vc.getMaVanChuyen() + "|" + vc.getMaDonHang() + "|" + to_string(vc.getNgayTao()) + "|" +
           vc.getTrangThaiGiao() + "|" + to_string(static_cast<long long>(vc.getChiPhiVanChuyen())) + "|" +
           vc.getLoaiXe() + "|" + to_string(vc.getThoiGianGiaoDuKien());
}
