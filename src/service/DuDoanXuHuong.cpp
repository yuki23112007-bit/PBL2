#include "DuDoanXuHuong.h"
#include "../models/NguoiDung.h"
#include <ctime>
#include <cmath>
#include <algorithm>
#include <set>

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

// QuickSort tu cai dat sap xep giam dan xac suat san pham xu huong thang sau
void DuDoanXuHuong::quickSortXuHuongSP(std::vector<KetQuaXuHuongSP>& arr, int low, int high) {
    if (low >= high) return;

    const auto& pivot = arr[high];
    double pProb = pivot.xacSuatDuocMuaNhieu;
    int p30 = pivot.soLuong30Ngay;
    int pTong = pivot.tongSoLuongBan;
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        bool lonHonHoacBang = false;
        if (arr[j].xacSuatDuocMuaNhieu > pProb) {
            lonHonHoacBang = true;
        } else if (std::abs(arr[j].xacSuatDuocMuaNhieu - pProb) < 1e-6) {
            if (arr[j].soLuong30Ngay > p30) {
                lonHonHoacBang = true;
            } else if (arr[j].soLuong30Ngay == p30 && arr[j].tongSoLuongBan >= pTong) {
                lonHonHoacBang = true;
            }
        }
        if (lonHonHoacBang) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    int p = i + 1;

    quickSortXuHuongSP(arr, low, p - 1);
    quickSortXuHuongSP(arr, p + 1, high);
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
    // Tap mau Huan luyen Xu huong San pham Thang sau (Phuong an 2 - Toan bo KH)
    // Dac trung: [TongSoLuongBan, SoKhachHangMua, SoDonHang, SoLuong30Ngay, DiemDanhGiaTB]
    // Nhan y: 1 = San pham xu huong (mua nhieu thang sau), 0 = Nhu cau thuong/thap
    // -------------------------------------------------------------
    std::vector<std::vector<double>> X2 = {
        // Nhom san pham xu huong, nhieu khach mua, tan suat cao -> y = 1
        { 12.0, 5.0, 5.0, 8.0, 4.8 },
        {  8.0, 4.0, 4.0, 6.0, 4.5 },
        { 15.0, 6.0, 6.0, 10.0, 4.7 },
        {  6.0, 3.0, 3.0, 4.0, 4.3 },
        {  5.0, 3.0, 3.0, 3.0, 4.2 },
        { 10.0, 4.0, 4.0, 7.0, 4.6 },
        {  4.0, 2.0, 3.0, 3.0, 4.4 },
        {  7.0, 3.0, 4.0, 5.0, 4.5 },
        // Nhom san pham it nguoi mua, khong on dinh -> y = 0
        {  1.0, 1.0, 1.0, 0.0, 3.0 },
        {  1.0, 1.0, 1.0, 1.0, 2.5 },
        {  2.0, 1.0, 1.0, 0.0, 3.2 },
        {  0.0, 0.0, 0.0, 0.0, 3.5 },
        {  2.0, 1.0, 1.0, 1.0, 2.8 },
        {  1.0, 1.0, 1.0, 0.0, 2.0 },
        {  3.0, 1.0, 1.0, 1.0, 3.0 },
        {  0.0, 0.0, 0.0, 0.0, 3.0 }
    };
    std::vector<int> y2 = { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

    // Tich hop them du lieu tu don hang thuc te neu co he thong
    if (qlDonHang && danhSachTaiKhoan) {
        time_t now = time(nullptr);
        const auto& dsDon = qlDonHang->getDanhSachDonHang();
        const auto& dsCTDH = qlDonHang->getDanhSachCTDH();
        const auto& dsTK = *danhSachTaiKhoan;

        // 1. Du lieu khach hang cho Model 1 (Quay lai)
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

        // 2. Du lieu tong hop san pham tu tat ca khach hang cho Model 2 (Xu huong SP thang sau)
        if (qlSanPham) {
            std::map<std::string, DonHang*> donHopLeMap;
            for (DonHang* dh : dsDon) {
                if (!dh) continue;
                std::string tt = dh->getTrangThaiDonHang();
                if (tt == "Đã hủy" || tt == "DA_HUY") continue;
                donHopLeMap[dh->getMaDonHang()] = dh;
            }

            struct ThongKeSP {
                int tongSL = 0;
                int sl30Ngay = 0;
                std::set<std::string> khachSet;
                std::set<std::string> donSet;
                double tongSao = 0.0;
                int demSao = 0;
            };
            std::map<std::string, ThongKeSP> tkSP;

            for (const ChiTietDonHang& ct : dsCTDH) {
                auto itDH = donHopLeMap.find(ct.getMaDonHang());
                if (itDH == donHopLeMap.end()) continue;
                DonHang* dh = itDH->second;

                auto& item = tkSP[ct.getMaSanPham()];
                item.tongSL += ct.getSoLuong();
                double tuoiNgay = difftime(now, dh->getThoiGianTao()) / 86400.0;
                if (tuoiNgay <= 30.0) {
                    item.sl30Ngay += ct.getSoLuong();
                }
                if (!dh->getMaNguoiDung().empty()) {
                    item.khachSet.insert(dh->getMaNguoiDung());
                }
                if (item.donSet.insert(dh->getMaDonHang()).second) {
                    if (diemDanhGiaMap.count(dh->getMaDonHang())) {
                        item.tongSao += diemDanhGiaMap[dh->getMaDonHang()];
                        item.demSao++;
                    }
                }
            }

            auto dsSP = qlSanPham->layDanhSachSanPham(0);
            for (SanPham* sp : dsSP) {
                if (!sp) continue;
                const auto& st = tkSP[sp->getMaSP()];
                int soKhach = static_cast<int>(st.khachSet.size());
                int soDon = static_cast<int>(st.donSet.size());
                double saoTB = st.demSao > 0 ? (st.tongSao / st.demSao) : 4.0;

                if (soDon >= 1) {
                    int nhanSP = (((soKhach >= 2 && st.tongSL >= 3) || st.sl30Ngay >= 3) && saoTB >= 3.5) ? 1 : 0;
                    X2.push_back({
                        static_cast<double>(st.tongSL),
                        static_cast<double>(soKhach),
                        static_cast<double>(soDon),
                        static_cast<double>(st.sl30Ngay),
                        saoTB
                    });
                    y2.push_back(nhanSP);
                }
            }
        }
    }

    soLuongMauHuanLuyen = static_cast<int>(X1.size());

    // Fit ca 2 mo hinh
    modelQuayLai.fit(X1, y1);
    modelXuHuongSP.fit(X2, y2);

    return modelQuayLai.isDaHuanLuyen() && modelXuHuongSP.isDaHuanLuyen();
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

std::vector<KetQuaXuHuongSP> DuDoanXuHuong::duDoanXuHuongSanPhamThangSau() {
    std::vector<KetQuaXuHuongSP> danhSach;
    if (!qlDonHang || !qlSanPham) return danhSach;

    time_t now = time(nullptr);
    const auto& dsDon = qlDonHang->getDanhSachDonHang();
    const auto& dsCTDH = qlDonHang->getDanhSachCTDH();

    // Tong hop du lieu tu TAT CA khach hang (bo qua don da huy)
    std::map<std::string, DonHang*> donHopLeMap;
    for (DonHang* dh : dsDon) {
        if (!dh) continue;
        std::string tt = dh->getTrangThaiDonHang();
        if (tt == "Đã hủy" || tt == "DA_HUY") continue;
        donHopLeMap[dh->getMaDonHang()] = dh;
    }

    struct ThongKeSP {
        int tongSL = 0;
        int sl30Ngay = 0;
        double doanhThu = 0.0;
        std::set<std::string> khachSet;
        std::set<std::string> donSet;
        double tongSao = 0.0;
        int demSao = 0;
    };
    std::map<std::string, ThongKeSP> tkSP;

    for (const ChiTietDonHang& ct : dsCTDH) {
        auto itDH = donHopLeMap.find(ct.getMaDonHang());
        if (itDH == donHopLeMap.end()) continue;
        DonHang* dh = itDH->second;

        auto& item = tkSP[ct.getMaSanPham()];
        item.tongSL += ct.getSoLuong();
        item.doanhThu += ct.getThanhTien();

        double tuoiNgay = difftime(now, dh->getThoiGianTao()) / 86400.0;
        if (tuoiNgay <= 30.0) {
            item.sl30Ngay += ct.getSoLuong();
        }
        if (!dh->getMaNguoiDung().empty()) {
            item.khachSet.insert(dh->getMaNguoiDung());
        }
        if (item.donSet.insert(dh->getMaDonHang()).second) {
            if (diemDanhGiaMap.count(dh->getMaDonHang())) {
                item.tongSao += diemDanhGiaMap[dh->getMaDonHang()];
                item.demSao++;
            }
        }
    }

    auto dsSP = qlSanPham->layDanhSachSanPham(0);
    double tongXacSuatTho = 0.0;

    for (SanPham* sp : dsSP) {
        if (!sp) continue;
        const auto& st = tkSP[sp->getMaSP()];

        KetQuaXuHuongSP kq;
        kq.maSanPham = sp->getMaSP();
        kq.tenSanPham = sp->getTenSP();

        std::string loai = sp->getLoaiSP();
        if (loai == "CONG_NGHE") kq.loaiSP = "Công nghệ";
        else if (loai == "DO_GIA_DUNG") kq.loaiSP = "Đồ gia dụng";
        else kq.loaiSP = "Thực phẩm";

        kq.soKhachMua = static_cast<int>(st.khachSet.size());
        kq.tongSoLuongBan = st.tongSL;
        kq.soLuong30Ngay = st.sl30Ngay;
        kq.soDonHang = static_cast<int>(st.donSet.size());
        kq.doanhThu = std::round(st.doanhThu);
        kq.diemDanhGiaTB = st.demSao > 0 ? std::round((st.tongSao / st.demSao) * 10.0) / 10.0 : 4.0;

        if (kq.soDonHang == 0) {
            kq.xacSuatDuocMuaNhieu = 0.0;
            kq.tyLeDuBao = 0.0;
            kq.xuHuong = "Chưa có dữ liệu";
            danhSach.push_back(kq);
            continue;
        }

        std::vector<double> x = {
            static_cast<double>(kq.tongSoLuongBan),
            static_cast<double>(kq.soKhachMua),
            static_cast<double>(kq.soDonHang),
            static_cast<double>(kq.soLuong30Ngay),
            kq.diemDanhGiaTB
        };
        std::vector<double> probas = modelXuHuongSP.predictProba(x);

        double pHot = 50.0;
        if (probas.size() >= 2) {
            pHot = probas[1] * 100.0;
        }

        kq.xacSuatDuocMuaNhieu = std::round(pHot * 10.0) / 10.0;
        tongXacSuatTho += kq.xacSuatDuocMuaNhieu;

        if (kq.xacSuatDuocMuaNhieu >= 75.0) kq.xuHuong = "Rất cao";
        else if (kq.xacSuatDuocMuaNhieu >= 50.0) kq.xuHuong = "Cao";
        else if (kq.xacSuatDuocMuaNhieu >= 25.0) kq.xuHuong = "Trung bình";
        else kq.xuHuong = "Thấp";

        kq.tyLeDuBao = 0.0;
        danhSach.push_back(kq);
    }

    // Chuan hoa ty trong du bao thang sau (tong = 100% tren toan bo san pham)
    if (tongXacSuatTho > 0.0) {
        for (auto& item : danhSach) {
            item.tyLeDuBao = std::round((item.xacSuatDuocMuaNhieu / tongXacSuatTho) * 1000.0) / 10.0;
        }
    }

    // QuickSort sap xep giam dan theo xac suat mua nhieu thang sau
    if (danhSach.size() > 1) {
        quickSortXuHuongSP(danhSach, 0, static_cast<int>(danhSach.size()) - 1);
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

    // Goi y nhom san pham ua thich dua tren lich su don hang cua khach (va fallback sang xu huong chung)
    kqNhom.idKhachHang = kqQuayLai.idKhachHang;
    kqNhom.hoTen = kqQuayLai.hoTen;
    kqNhom.username = kqQuayLai.username;

    double tienThucPham = 0.0, tienCongNghe = 0.0, tienGiaDung = 0.0;
    if (qlDonHang && qlSanPham) {
        const auto& dsDon = qlDonHang->getDanhSachDonHang();
        const auto& dsCTDH = qlDonHang->getDanhSachCTDH();
        for (DonHang* dh : dsDon) {
            if (dh && dh->getMaNguoiDung() == idKhachHang) {
                for (const ChiTietDonHang& ct : dsCTDH) {
                    if (ct.getMaDonHang() == dh->getMaDonHang()) {
                        SanPham* sp = qlSanPham->timSanPhamTheoMa(ct.getMaSanPham());
                        if (sp) {
                            std::string loai = sp->getLoaiSP();
                            if (loai == "CONG_NGHE") tienCongNghe += ct.getThanhTien();
                            else if (loai == "DO_GIA_DUNG") tienGiaDung += ct.getThanhTien();
                            else tienThucPham += ct.getThanhTien();
                        } else {
                            tienThucPham += ct.getThanhTien();
                        }
                    }
                }
            }
        }
    }

    double tongTien = tienThucPham + tienCongNghe + tienGiaDung;
    if (tongTien > 0.0) {
        double p0 = std::round((tienThucPham / tongTien) * 1000.0) / 10.0;
        double p1 = std::round((tienCongNghe / tongTien) * 1000.0) / 10.0;
        double p2 = std::round((tienGiaDung / tongTien) * 1000.0) / 10.0;
        kqNhom.xacSuatCacNhom = { p0, p1, p2 };
        if (p1 >= p0 && p1 >= p2) {
            kqNhom.nhomDuDoan = "Công nghệ";
            kqNhom.xacSuatNhom = p1;
        } else if (p2 >= p0 && p2 >= p1) {
            kqNhom.nhomDuDoan = "Đồ gia dụng";
            kqNhom.xacSuatNhom = p2;
        } else {
            kqNhom.nhomDuDoan = "Thực phẩm";
            kqNhom.xacSuatNhom = p0;
        }
    } else {
        kqNhom.nhomDuDoan = "Thực phẩm";
        kqNhom.xacSuatNhom = 100.0;
        kqNhom.xacSuatCacNhom = { 100.0, 0.0, 0.0 };
    }

    return true;
}
