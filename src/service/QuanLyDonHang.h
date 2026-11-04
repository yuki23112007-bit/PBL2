#ifndef QUANLYDONHANG_H
#define QUANLYDONHANG_H

#include "../models/DonHang.h"
#include "../models/ChiTietDonHang.h"
#include "../core/OrderQueue.h"
#include "../core/BinarySearchTree.h"
#include "QuanLySanPham.h"
#include <string>
#include <vector>

using namespace std;

class QuanLyDonHang {
    private:
        OrderQueue hangDoiDonHang;              // Hang doi FIFO + Pending List
        BinarySearchTree cayDonHang;            // BST tra cuu nhan dien O(log N)
        vector<DonHang*> danhSachDonHang;       // So huu bo nho cac doi tuong DonHang*
        vector<ChiTietDonHang> danhSachCTDH;
        int demDonHang;
        int demChiTiet;

        string taoMaDonHangMoi();
        string taoMaChiTietMoi();

    public:
        QuanLyDonHang();
        ~QuanLyDonHang();

        QuanLyDonHang(const QuanLyDonHang&) = delete;
        QuanLyDonHang& operator=(const QuanLyDonHang&) = delete;

        bool datHang(const string& maND, QuanLySanPham& qlSP,
                     const string& maSP, int soLuong,
                     const string& tenKH, const string& diaChi,
                     const string& sdt, string& maDHMoi);

        bool duyetDonHangTiepTheo(QuanLySanPham& qlSP, string& thongBao);
        void kiemTraVaHuyDonTimeout();
        bool huyDonHang(const string& maDH, const string& maND = "");

        void xemLichSuDonHang(const string& maND) const;
        void xemDanhSachDonHangBST() const;
        DonHang* timDonHangTheoMa(const string& maDH) const;
        void xemHangDoi() const;
        void xemDanhSachPending() const;
        bool phucHoiDonPending(const string& maDH, QuanLySanPham& qlSP);

        OrderQueue& getHangDoi();
        BinarySearchTree& getCayDonHang();
        const vector<DonHang*>& getDanhSachDonHang() const;
        const vector<ChiTietDonHang>& getDanhSachCTDH() const;

        // Phuong thuc ho tro nap don tu file
        void themDonHangCoSan(DonHang* dh, const vector<ChiTietDonHang>& chiTiets);
};

#endif // QUANLYDONHANG_H
