#include "DuDoanXuHuong.h"
#include "../models/NguoiDung.h"
#include <ctime>
#include <cmath>
#include <algorithm>

DuDoanXuHuong::DuDoanXuHuong()
    : qlDonHang(nullptr), danhSachTaiKhoan(nullptr), qlSanPham(nullptr),
      soLuongMauHuanLuyen(0), thoiGianHuanLuyen(0) {}

DuDoanXuHuong::DuDoanXuHuong(QuanLyDonHang* qlDH, const std::vector<TaiKhoan*>* dsTK, QuanLySanPham* qlSP)
    : qlDonHang(qlDH), danhSachTaiKhoan(dsTK), qlSanPham(qlSP),
      soLuongMauHuanLuyen(0), thoiGianHuanLuyen(0) {
    huanLuyen();
}

DuDoanXuHuong::~DuDoanXuHuong() {}

void DuDoanXuHuong::ganDependencies(QuanLyDonHang* qlDH, const std::vector<TaiKhoan*>* dsTK, QuanLySanPham* qlSP) {
    qlDonHang = qlDH;
    danhSachTaiKhoan = dsTK;
    qlSanPham = qlSP;
    huanLuyen();
}

void DuDoanXuHuong::dongBoDanhGia(const std::vector<DanhGiaDonHang>& dsDG) {
    for (const auto& dg : dsDG) {
        diemDanhGiaMap[dg.getMaDonHang()] = static_cast<double>(dg.getSoSao());
    }
}

void DuDoanXuHuong::capNhatDanhGia(const std::string& maDH, double soSao) {
    diemDanhGiaMap[maDH] = soSao;
}

// QuickSort tu cai dat sap xep giam dan xac suat quay lai
void DuDoanXuHuong::quickSortKetQua(std::vector<KetQuaQuayLai>& arr, int low, int high) {
    if (low >= high) return;

    double pivot = arr[high].xacSuatQuayLai;
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        // Giam dan: lon hon hoac bang pivot thi dua ve truoc
        if (arr[j].xacSuatQuayLai >= pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    int p = i + 1;

    quickSortKetQua(arr, low, p - 1);
    quickSortKetQua(arr, p + 1, high);
}

bool DuDoanXuHuong::huanLuyen() {
    thoiGianHuanLuyen = time(nullptr);

    // -------------------------------------------------------------
    // Tap mau Huan luyen chuan (Baseline Standard RFM Distributions)
    // Dac trung [Recency(ngay), Frequency(don), Monetary(VND), Satisfaction(sao)]
    // Nhan y: 1 = Quay lai mua tiep, 0 = Khong quay lai (churn)
    // -------------------------------------------------------------
    std::vector<std::vector<double>> X1 = {
        // Nhom khach hang than thiet, hai long cao -> Chac chan quay lai (y = 1)
        { 2.0,  8.0, 450000.0, 5.0 },
        { 5.0,  6.0, 320000.0, 4.5 },
        { 7.0,  5.0, 680000.0, 5.0 },
        { 4.0, 10.0, 510000.0, 4.8 },
        { 10.0, 4.0, 290000.0, 4.2 },
        { 3.0,  7.0, 410000.0, 4.7 },
        { 8.0,  5.0, 380000.0, 4.0 },
        { 12.0, 3.0, 250000.0, 4.0 },
        // Nhom khach hang it mua, lau khong mua, danh gia thap -> Khong quay lai (y = 0)
        { 45.0, 1.0,  80000.0, 2.0 },
        { 60.0, 1.0, 120000.0, 1.0 },
        { 75.0, 2.0, 150000.0, 2.5 },
        { 90.0, 1.0,  50000.0, 2.0 },
        { 40.0, 2.0, 110000.0, 3.0 },
        { 80.0, 1.0,  95000.0, 1.5 },
        { 55.0, 1.0, 140000.0, 2.2 },
        { 100.0, 1.0, 70000.0, 1.0 }
    };
    std::vector<int> y1 = { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

    // -------------------------------------------------------------
    // Tap mau Huan luyen Nhom san pham tiep theo (Phuong an 2)
    // Dac trung: [TyLeThucPham, TyLeCongNghe, TyLeGiaDung, ChiTieuTB, TanSuat]
    // Nhan y: 0 = Thuc pham, 1 = Cong nghe, 2 = Do gia dung
    // -------------------------------------------------------------
    std::vector<std::vector<double>> X2 = {
        // Uu tien Thuc pham (y = 0)
        { 0.85, 0.05, 0.10, 120000.0, 5.0 },
        { 0.75, 0.10, 0.15, 180000.0, 4.0 },
        { 0.90, 0.00, 0.10,  95000.0, 6.0 },
        { 0.70, 0.15, 0.15, 140000.0, 3.0 },
        // Uu tien Cong nghe (y = 1)
        { 0.10, 0.80, 0.10, 1250000.0, 2.0 },
        { 0.15, 0.75, 0.10,  980000.0, 2.0 },
        { 0.05, 0.85, 0.10, 2100000.0, 1.0 },
        { 0.20, 0.70, 0.10,  850000.0, 3.0 },
        // Uu tien Do gia dung (y = 2)
        { 0.15, 0.10, 0.75, 380000.0, 2.0 },
        { 0.10, 0.15, 0.75, 450000.0, 3.0 },
        { 0.20, 0.05, 0.75, 290000.0, 2.0 },
        { 0.05, 0.10, 0.85, 520000.0, 2.0 }
    };
    std::vector<int> y2 = { 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2 };

    // Tich hop them du lieu tu don hang thuc te neu co he thong
    if (qlDonHang && danhSachTaiKhoan) {
        time_t now = time(nullptr);
        const auto& dsDon = qlDonHang->getDanhSachDonHang();
        const auto& dsCTDH = qlDonHang->getDanhSachCTDH();
        const auto& dsTK = *danhSachTaiKhoan;

        for (TaiKhoan* tk : dsTK) {
            if (!tk || tk->getVaiTro() == "ADMIN") continue;
            std::string idND = tk->getId();

            int donHoanThanh = 0;
            double tongTien = 0.0;
            time_t donMoiNhat = 0;
            double tongSao = 0.0;
            int demSao = 0;

            for (DonHang* dh : dsDon) {
                if (dh && dh->getMaNguoiDung() == idND) {
                    if (dh->getTrangThaiDonHang() == "Đã hoàn thành" || dh->getTrangThaiDonHang() == "DA_HOAN_THANH") {
                        donHoanThanh++;
                        if (dh->getThoiGianTao() > donMoiNhat) donMoiNhat = dh->getThoiGianTao();

                        // Tinh tien
                        for (const ChiTietDonHang& ct : dsCTDH) {
                            if (ct.getMaDonHang() == dh->getMaDonHang()) {
                                tongTien += ct.getThanhTien();
                            }
                        }

                        // Danh gia
                        if (diemDanhGiaMap.count(dh->getMaDonHang())) {
                            tongSao += diemDanhGiaMap[dh->getMaDonHang()];
                            demSao++;
                        }
                    }
                }
            }

            if (donHoanThanh >= 1) {
                double recency = donMoiNhat > 0 ? (difftime(now, donMoiNhat) / 86400.0) : 15.0;
                if (recency < 0.1) recency = 0.1;
                double aov = tongTien / donHoanThanh;
                double saoTB = demSao > 0 ? (tongSao / demSao) : 4.0;

                int nhan = (recency <= 30.0 && saoTB >= 3.0) ? 1 : 0;
                X1.push_back({ recency, static_cast<double>(donHoanThanh), aov, saoTB });
                y1.push_back(nhan);
            }
        }
    }

    soLuongMauHuanLuyen = static_cast<int>(X1.size());

    // Fit ca 2 mo hinh
    modelQuayLai.fit(X1, y1);
    modelNhomSP.fit(X2, y2);

    return modelQuayLai.isDaHuanLuyen() && modelNhomSP.isDaHuanLuyen();
}

std::vector<KetQuaQuayLai> DuDoanXuHuong::duDoanTatCaQuayLai() {
    std::vector<KetQuaQuayLai> danhSach;
    if (!danhSachTaiKhoan || !qlDonHang) return danhSach;

    time_t now = time(nullptr);
    const auto& dsTK = *danhSachTaiKhoan;
    const auto& dsDon = qlDonHang->getDanhSachDonHang();
    const auto& dsCTDH = qlDonHang->getDanhSachCTDH();

    for (TaiKhoan* tk : dsTK) {
        if (!tk || tk->getVaiTro() == "ADMIN") continue;
        NguoiDung* nd = dynamic_cast<NguoiDung*>(tk);
        std::string idND = tk->getId();

        int soDon = 0;
        double tongTien = 0.0;
        time_t donCuoi = 0;
        double tongSao = 0.0;
        int demSao = 0;

        for (DonHang* dh : dsDon) {
            if (dh && dh->getMaNguoiDung() == idND) {
                soDon++;
                if (dh->getThoiGianTao() > donCuoi) donCuoi = dh->getThoiGianTao();

                for (const ChiTietDonHang& ct : dsCTDH) {
                    if (ct.getMaDonHang() == dh->getMaDonHang()) {
                        tongTien += ct.getThanhTien();
                    }
                }

                if (diemDanhGiaMap.count(dh->getMaDonHang())) {
                    tongSao += diemDanhGiaMap[dh->getMaDonHang()];
                    demSao++;
                }
            }
        }

        KetQuaQuayLai kq;
        kq.idKhachHang = idND;
        kq.username = tk->getUsername();
        kq.hoTen = nd ? nd->getHoTen() : tk->getUsername();
        kq.frequencyDon = soDon;

        if (soDon == 0) {
            kq.recencyNgay = 0.0;
            kq.monetaryAOV = 0.0;
            kq.satisfactionSao = 0.0;
            kq.xacSuatQuayLai = 0.0;
            kq.mucDo = "Chưa có đơn";
            danhSach.push_back(kq);
            continue;
        }

        // Tinh cac dac trung RFM thuc te
        double recency = donCuoi > 0 ? (difftime(now, donCuoi) / 86400.0) : 0.0;
        if (recency < 0.1) recency = 0.1;
        double aov = tongTien / soDon;
        double satisfaction = demSao > 0 ? (tongSao / demSao) : 4.0;

        std::vector<double> x = { recency, static_cast<double>(soDon), aov, satisfaction };
        std::vector<double> probas = modelQuayLai.predictProba(x);

        double pQuayLai = 50.0;
        if (probas.size() >= 2) {
            pQuayLai = probas[1] * 100.0;
        }

        kq.xacSuatQuayLai = std::round(pQuayLai * 10.0) / 10.0;
        kq.recencyNgay = std::round(recency * 10.0) / 10.0;
        kq.monetaryAOV = std::round(aov);
        kq.satisfactionSao = std::round(satisfaction * 10.0) / 10.0;

        if (kq.xacSuatQuayLai >= 70.0) kq.mucDo = "Cao";
        else if (kq.xacSuatQuayLai >= 40.0) kq.mucDo = "Trung bình";
        else kq.mucDo = "Thấp";

        danhSach.push_back(kq);
    }

    // QuickSort sap xep giam dan xac suat
    if (danhSach.size() > 1) {
        quickSortKetQua(danhSach, 0, static_cast<int>(danhSach.size()) - 1);
    }

    return danhSach;
}

std::vector<KetQuaNhomSP> DuDoanXuHuong::duDoanTatCaNhomSP() {
    std::vector<KetQuaNhomSP> danhSach;
    if (!danhSachTaiKhoan || !qlDonHang || !qlSanPham) return danhSach;

    const auto& dsTK = *danhSachTaiKhoan;
    const auto& dsDon = qlDonHang->getDanhSachDonHang();
    const auto& dsCTDH = qlDonHang->getDanhSachCTDH();

    for (TaiKhoan* tk : dsTK) {
        if (!tk || tk->getVaiTro() == "ADMIN") continue;
        NguoiDung* nd = dynamic_cast<NguoiDung*>(tk);
        std::string idND = tk->getId();

        double tienThucPham = 0.0;
        double tienCongNghe = 0.0;
        double tienGiaDung = 0.0;
        int tongDon = 0;

        for (DonHang* dh : dsDon) {
            if (dh && dh->getMaNguoiDung() == idND) {
                tongDon++;
                for (const ChiTietDonHang& ct : dsCTDH) {
                    if (ct.getMaDonHang() == dh->getMaDonHang()) {
                        SanPham* sp = qlSanPham->timSanPhamTheoMa(ct.getMaSanPham());
                        if (sp) {
                            std::string loai = sp->getLoaiSP();
                            if (loai == "THUONG" || loai == "TUOI_SONG") tienThucPham += ct.getThanhTien();
                            else if (loai == "CONG_NGHE") tienCongNghe += ct.getThanhTien();
                            else tienGiaDung += ct.getThanhTien();
                        } else {
                            tienThucPham += ct.getThanhTien();
                        }
                    }
                }
            }
        }

        KetQuaNhomSP kq;
        kq.idKhachHang = idND;
        kq.username = tk->getUsername();
        kq.hoTen = nd ? nd->getHoTen() : tk->getUsername();

        double tongTien = tienThucPham + tienCongNghe + tienGiaDung;

        if (tongDon == 0 || tongTien <= 0.0) {
            kq.nhomDuDoan = "Chưa có dữ liệu";
            kq.xacSuatNhom = 0.0;
            kq.xacSuatCacNhom = { 0.0, 0.0, 0.0 };
            danhSach.push_back(kq);
            continue;
        }

        double r0 = tienThucPham / tongTien;
        double r1 = tienCongNghe / tongTien;
        double r2 = tienGiaDung / tongTien;
        double aov = tongTien / tongDon;
        double freq = static_cast<double>(tongDon);

        std::vector<double> x = { r0, r1, r2, aov, freq };
        std::vector<double> probas = modelNhomSP.predictProba(x);

        if (probas.size() < 3) {
            probas = { 0.34, 0.33, 0.33 };
        }

        int maxIdx = 0;
        if (probas[1] > probas[maxIdx]) maxIdx = 1;
        if (probas[2] > probas[maxIdx]) maxIdx = 2;

        std::string tenNhom = "Thực phẩm";
        if (maxIdx == 1) tenNhom = "Công nghệ";
        else if (maxIdx == 2) tenNhom = "Đồ gia dụng";

        kq.nhomDuDoan = tenNhom;
        kq.xacSuatNhom = std::round(probas[maxIdx] * 1000.0) / 10.0;
        kq.xacSuatCacNhom = {
            std::round(probas[0] * 1000.0) / 10.0,
            std::round(probas[1] * 1000.0) / 10.0,
            std::round(probas[2] * 1000.0) / 10.0
        };

        danhSach.push_back(kq);
    }

    return danhSach;
}

bool DuDoanXuHuong::duDoanChoKhachHang(const std::string& idKhachHang, KetQuaQuayLai& kqQuayLai, KetQuaNhomSP& kqNhom) {
    auto dsQuayLai = duDoanTatCaQuayLai();
    bool timThay = false;
    for (const auto& item : dsQuayLai) {
        if (item.idKhachHang == idKhachHang) {
            if (item.frequencyDon == 0) {
                // Khach chua co don hang nao thi khong du doan
                return false;
            }
            kqQuayLai = item;
            timThay = true;
            break;
        }
    }
    if (!timThay) return false;

    auto dsNhom = duDoanTatCaNhomSP();
    for (const auto& item : dsNhom) {
        if (item.idKhachHang == idKhachHang) {
            kqNhom = item;
            return true;
        }
    }

    return false;
}
