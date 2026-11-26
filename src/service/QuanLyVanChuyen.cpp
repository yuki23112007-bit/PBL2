#include "QuanLyVanChuyen.h"
#include "TinhKhoangCachDiaLy.h"
#include "../models/PhuongTien.h"
#include "../models/XeTai.h"
#include "../models/XeTaiLanh.h"
#include "../models/XeMay.h"
#include "../models/VanChuyen.h"

using namespace std;

long long QuanLyVanChuyen::tinhSoGiayGiaoDuKien(double quangDuongKm, bool coHangTuoiSong, string& nhanThoiGianGiao) {
    if (quangDuongKm <= 30.0) {
        if (coHangTuoiSong) {
            nhanThoiGianGiao = "Trong ngày (~6 giờ, nội thành)";
            return 6LL * 3600;
        }
        nhanThoiGianGiao = "1 ngày (Nội thành)";
        return 1LL * 86400;
    }
    if (quangDuongKm <= 200.0) {
        nhanThoiGianGiao = "2 ngày (Lân cận)";
        return 2LL * 86400;
    }
    if (quangDuongKm <= 700.0) {
        nhanThoiGianGiao = "3 ngày (Liên vùng)";
        return 3LL * 86400;
    }
    if (quangDuongKm <= 1200.0) {
        nhanThoiGianGiao = "4 ngày (Liên vùng xa)";
        return 4LL * 86400;
    }
    nhanThoiGianGiao = "5 ngày (Bắc - Nam)";
    return 5LL * 86400;
}

double QuanLyVanChuyen::tinhPhiVanChuyenTheoXe(double quangDuongKm, double tienHang, bool coHangTuoiSong, string& loaiXeRa) {
    PhuongTien* pt = nullptr;
    if (coHangTuoiSong) {
        pt = new XeTaiLanh("43C-88888", "Hino", 2.5, -18.0);
    } else if (tienHang >= 1000000.0 || quangDuongKm > 30.0) {
        pt = new XeTai("43C-66666", "Isuzu", 1.5);
    } else {
        pt = new XeMay("43D1-12345", "Honda", 125);
    }

    VanChuyen vc("VC-EST", "", time(nullptr), "Đang giao", 0.0, "", 0, pt);
    double phi = vc.tinhChiPhiVanChuyen(quangDuongKm);
    loaiXeRa = pt->getLoaiPhuongTien();
    delete pt;
    return phi;
}

UocTinhVanChuyen QuanLyVanChuyen::uocTinhChoSanPham(const string& maSP,
                                                    double tienHang,
                                                    const string& diaChiKhach,
                                                    QuanLySanPham& qlSanPham,
                                                    const QuanLyKhoNCC& qlKhoNCC) {
    UocTinhVanChuyen kq;
    kq.maKho = "KHO02";
    kq.thanhPhoKho = "Đà Nẵng";
    kq.thanhPhoKhach = "Khu vực khác";
    kq.quangDuongKm = 50.0;
    kq.phiVanChuyen = 40000.0;
    kq.soGiayGiaoDuKien = 2LL * 86400;
    kq.nhanThoiGianGiao = "2 ngày (Liên tỉnh)";

    SanPham* sp = qlSanPham.timSanPhamTheoMa(maSP);
    bool coHangTuoiSong = (sp && sp->getLoaiSP() == "TUOI_SONG");

    const ToaDoThanhPho* tpKhach = TinhKhoangCachDiaLy::timThanhPhoTuDiaChi(diaChiKhach);
    if (tpKhach) kq.thanhPhoKhach = tpKhach->tenChuan;

    // 1. Chon kho xuat hang: uu tien kho luu truc tiep maSP, neu khong chon kho phu hop gan khach nhat
    const auto& dsKho = qlKhoNCC.getDanhSachKho();
    const KhoHang* khoChon = nullptr;
    const ToaDoThanhPho* tpKhoChon = nullptr;

    for (const KhoHang* k : dsKho) {
        if (k && !maSP.empty() && k->getMaSanPham() == maSP) {
            khoChon = k;
            tpKhoChon = TinhKhoangCachDiaLy::timThanhPhoTuDiaChi(k->getViTri());
            break;
        }
    }

    if (!khoChon && !dsKho.empty()) {
        double minDist = 1e18;
        auto xetKho = [&](bool chiKhoLanh) {
            for (const KhoHang* k : dsKho) {
                if (!k) continue;
                if (chiKhoLanh) {
                    string vt = TinhKhoangCachDiaLy::chuanHoaChuThuongUtf8(k->getViTri());
                    if (vt.find("lạnh") == string::npos && vt.find("lanh") == string::npos) continue;
                }
                const ToaDoThanhPho* tpK = TinhKhoangCachDiaLy::timThanhPhoTuDiaChi(k->getViTri());
                double dist = (tpK && tpKhach) ? TinhKhoangCachDiaLy::tinhKhoangCachKm(*tpK, *tpKhach) : 50.0;
                if (dist < minDist) {
                    minDist = dist;
                    khoChon = k;
                    tpKhoChon = tpK;
                }
            }
        };
        if (coHangTuoiSong) xetKho(true);
        if (!khoChon) xetKho(false);
    }

    if (khoChon) {
        kq.maKho = khoChon->getMaKho();
        if (tpKhoChon) kq.thanhPhoKho = tpKhoChon->tenChuan;
    }

    if (tpKhoChon && tpKhach) {
        kq.quangDuongKm = TinhKhoangCachDiaLy::tinhKhoangCachKm(*tpKhoChon, *tpKhach);
    } else {
        kq.quangDuongKm = 50.0;
    }

    // 2. Dieu phoi phuong tien & tinh phi ship da hinh
    kq.phiVanChuyen = tinhPhiVanChuyenTheoXe(kq.quangDuongKm, tienHang, coHangTuoiSong, kq.loaiXe);

    // 3. Tinh thoi gian giao du kien theo khoang cach vung
    kq.soGiayGiaoDuKien = tinhSoGiayGiaoDuKien(kq.quangDuongKm, coHangTuoiSong, kq.nhanThoiGianGiao);

    return kq;
}

UocTinhVanChuyen QuanLyVanChuyen::uocTinhChoDonHang(const DonHang* dh,
                                                    const vector<ChiTietDonHang>& dsCTDH,
                                                    QuanLySanPham& qlSanPham,
                                                    const QuanLyKhoNCC& qlKhoNCC) {
    if (!dh) return uocTinhChoSanPham("", 0.0, "", qlSanPham, qlKhoNCC);

    string maSPChinh;
    double tienHang = 0.0;
    for (const ChiTietDonHang& ct : dsCTDH) {
        if (ct.getMaDonHang() == dh->getMaDonHang()) {
            tienHang += ct.getThanhTien();
            SanPham* sp = qlSanPham.timSanPhamTheoMa(ct.getMaSanPham());
            if (maSPChinh.empty() || (sp && sp->getLoaiSP() == "TUOI_SONG")) {
                maSPChinh = ct.getMaSanPham();
            }
        }
    }
    return uocTinhChoSanPham(maSPChinh, tienHang, dh->getDiaChiKhachHang(), qlSanPham, qlKhoNCC);
}
