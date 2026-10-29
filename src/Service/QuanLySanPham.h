#ifndef QUANLYSANPHAM_H
#define QUANLYSANPHAM_H

#include "../models/SanPham.h"
#include "../models/ThucPham.h"
#include "../core/HashTable.h"
#include "../core/Trie.h"
#include "../core/MinHeap.h"
#include <string>
#include <vector>

using namespace std;

class QuanLySanPham {
    private:
        HashTable bangBamSanPham;  // Kho luu tru goc duy nhat so huu SanPham*
        Trie cayTienToSanPham;     // Cay tien to phuc vu Autocomplete
        MinHeap heapTuoiSong;      // Min-Heap quan ly han dung hang tuoi song

        // Cac ham sap xep QuickSort viet tay (tuan thu doc1 muc 10.1)
        void quickSortGia(vector<SanPham*>& arr, int low, int high, bool tangDan);
        void quickSortTonKho(vector<SanPham*>& arr, int low, int high, bool tangDan);
        void quickSortTen(vector<SanPham*>& arr, int low, int high);

    public:
        QuanLySanPham(int dungLuongBangBam = 101);
        ~QuanLySanPham() = default;

        QuanLySanPham(const QuanLySanPham&) = delete;
        QuanLySanPham& operator=(const QuanLySanPham&) = delete;

        bool themSanPham(SanPham* sp);
        bool capNhatSoLuongKho(const string& maSP, int soLuongThayDoi);
        bool xoaSanPham(const string& maSP);

        SanPham* timSanPhamTheoMa(const string& maSP);
        vector<SanPham*> timKiemTheoTen(const string& tuKhoa);
        vector<SanPham*> layDanhSachSanPham(int tieuChiSapXep = 0);

        void kiemTraCanhBaoHetHang(int nguong = 10) const;
        vector<ThucPham*> kiemTraCanhBaoHetHan(int soNgay = 2) const;

        void hienThiToanBo() const;

        HashTable& getBangBam();
        Trie& getCayTienTo();
        MinHeap& getMinHeap();
};

#endif // QUANLYSANPHAM_H
