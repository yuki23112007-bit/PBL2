#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <limits>

#include "../models/SanPham.h"
#include "../models/SanPhamThuong.h"
#include "../models/SanPhamTuoiSong.h"
#include "../models/KhoHang.h"
#include "../models/DonHang.h"
#include "../models/ChiTietDonHang.h"
#include "../core/HashTable.h"
#include "../core/Trie.h"
#include "../core/BinarySearchTree.h"
#include "../core/MinHeap.h"
#include "../core/OrderQueue.h"
#include "../Service/QuanLySanPham.h"
#include "../Service/QuanLyDonHang.h"

using namespace std;

// Ham xoa bo dem cin chong troi lenh
void xoaBoDem() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// 1. Khoi tao 5 kho hang mau
void khoiTaoKhoHang(vector<KhoHang>& danhSachKho) {
    danhSachKho.push_back(KhoHang("KHO01", "Kho Tong Ha Noi", 10000, "SP01"));
    danhSachKho.push_back(KhoHang("KHO02", "Kho Lanh Da Nang", 5000, "SP12"));
    danhSachKho.push_back(KhoHang("KHO03", "Kho Nông San Can Tho", 8000, "SP06"));
    danhSachKho.push_back(KhoHang("KHO04", "Kho Lanh TP. Ho Chi Minh", 12000, "SP13"));
    danhSachKho.push_back(KhoHang("KHO05", "Kho Cang Hai Phong", 7000, "SP14"));
}

// 2. Khoi tao tren 20 san pham mau (gom ca SanPhamThuong va SanPhamTuoiSong)
void khoiTaoSanPham(QuanLySanPham& qlSP) {
    time_t now = time(nullptr);

    // --- 11 San pham thuong (Do kho, gia dung, luong thuc) ---
    qlSP.themSanPham(new SanPhamThuong("SP01", "Gao ST25 Ong Cua", "Luong thuc", 35000, 150, "NCC01", now - 15 * 86400, 365, "Bao 5kg"));
    qlSP.themSanPham(new SanPhamThuong("SP02", "Mi Hao Hao Tom Chua Cay", "Mi an lien", 4500, 800, "NCC02", now - 20 * 86400, 180, "Thung 30 goi"));
    qlSP.themSanPham(new SanPhamThuong("SP03", "Dau An Neptune Light", "Gia vi", 58000, 90, "NCC01", now - 30 * 86400, 720, "Chai 1L"));
    qlSP.themSanPham(new SanPhamThuong("SP04", "Nuoc Mam Nam Ngu De Nhi", "Gia vi", 32000, 120, "NCC03", now - 25 * 86400, 365, "Chai 750ml"));
    qlSP.themSanPham(new SanPhamThuong("SP05", "Duong Tinh Luyen Bien Hoa", "Gia vi", 28000, 60, "NCC01", now - 40 * 86400, 540, "Goi 1kg"));
    qlSP.themSanPham(new SanPhamThuong("SP06", "Sua Tuoi Vinamilk Khong Duong", "Sua", 36000, 40, "NCC04", now - 10 * 86400, 180, "Loc 4 hop"));
    qlSP.themSanPham(new SanPhamThuong("SP07", "Ca Phe Hoa Tan G7", "Do uong", 62000, 75, "NCC05", now - 12 * 86400, 730, "Hop 21 goi"));
    qlSP.themSanPham(new SanPhamThuong("SP08", "Banh Quy Cosy Marie", "Banh keo", 42000, 85, "NCC02", now - 18 * 86400, 365, "Hop 336g"));
    qlSP.themSanPham(new SanPhamThuong("SP09", "Tra Xanh Khong Do Chai", "Do uong", 10000, 200, "NCC05", now - 10 * 86400, 365, "Chai 455ml"));
    // San pham it hang de test canh bao ton kho (<= 10)
    qlSP.themSanPham(new SanPhamThuong("SP10", "Tuong Ot Chin-su Chai", "Gia vi", 16000, 9, "NCC03", now - 45 * 86400, 270, "Chai 250g"));
    qlSP.themSanPham(new SanPhamThuong("SP11", "Gao Jasmine Thom Thuong Hang", "Luong thuc", 24000, 5, "NCC01", now - 50 * 86400, 365, "Bao 5kg"));

    // --- 10 San pham tuoi song (Thit, ca, hai san, sua lanh) ---
    // SP12: Han dung con 1 ngay nua -> Test MinHeap canh bao het han
    qlSP.themSanPham(new SanPhamTuoiSong("SP12", "Ca Hoi Nauy Fillet Tuoi", "Hai san", 135000, 25, "NCC06", now - 4 * 86400, 5, -2.5));
    qlSP.themSanPham(new SanPhamTuoiSong("SP13", "Thit Ba Chi Bo My Dong Lanh", "Thit tuoi", 185000, 30, "NCC06", now - 5 * 86400, 30, -18.0));
    // SP14: Con 2 ngay nua het han -> Test MinHeap
    qlSP.themSanPham(new SanPhamTuoiSong("SP14", "Tom Su Bien Tuoi Song", "Hai san", 220000, 18, "NCC06", now - 2 * 86400, 4, -1.0));
    qlSP.themSanPham(new SanPhamTuoiSong("SP15", "Thit Nac Dam Heo CP", "Thit tuoi", 95000, 45, "NCC07", now - 1 * 86400, 4, 2.0));
    qlSP.themSanPham(new SanPhamTuoiSong("SP16", "Muc Ong Phu Quoc Tuoi", "Hai san", 160000, 8, "NCC06", now - 2 * 86400, 6, -4.0));
    qlSP.themSanPham(new SanPhamTuoiSong("SP17", "Sua Chua Vinamilk Co Duong", "Sua lanh", 28000, 50, "NCC04", now - 10 * 86400, 45, 4.0));
    qlSP.themSanPham(new SanPhamTuoiSong("SP18", "Ga Ta Tha Vuon Lam Dong", "Thit gia cam", 115000, 22, "NCC07", now - 1 * 86400, 4, 0.0));
    qlSP.themSanPham(new SanPhamTuoiSong("SP19", "Rau Cai Ngot Da Lat", "Rau cu", 18000, 35, "NCC08", now - 1 * 86400, 5, 5.0));
    // SP20: Vua sap het han (con 1 ngay) vua ton kho = 4 (test ca 2 canh bao!)
    qlSP.themSanPham(new SanPhamTuoiSong("SP20", "Dau Phu Sach Vissan", "Thuc pham mat", 12000, 4, "NCC07", now - 2 * 86400, 3, 4.0));
    qlSP.themSanPham(new SanPhamTuoiSong("SP21", "Cua Bien Ca Mau Tuoi Song", "Hai san", 350000, 12, "NCC06", now - 1 * 86400, 3, 2.0));
}

// 3. Khoi tao don hang mau de test Hieu nang Queue, BST va Pending List
void khoiTaoDonHang(QuanLyDonHang& qlDH, QuanLySanPham& qlSP) {
    string maMoi;
    // Don 1: Mua SP01 (so luong 2, du kho)
    qlDH.datHang("KH01", qlSP, "SP01", 2, "Nguyen Van An", "12 Hai Ba Trung, Ha Noi", "0912345678", maMoi);

    // Don 2: Mua SP12 (so luong 1, du kho)
    qlDH.datHang("KH02", qlSP, "SP12", 1, "Tran Thi Binh", "45 Le Duan, Da Nang", "0987654321", maMoi);

    // Don 3: Mua SP11 (so luong 20, trong khi ton kho chi co 5 -> Thieu kho de test Pending List chống nghẽn!)
    qlDH.datHang("KH03", qlSP, "SP11", 20, "Le Van Cuong", "88 Nguyen Hue, TP.HCM", "0905112233", maMoi);

    // Don 4: Mua SP02 (so luong 5, du kho -> dung sau don 3 de chung minh khong bi nghen!)
    qlDH.datHang("KH01", qlSP, "SP02", 5, "Nguyen Van An", "12 Hai Ba Trung, Ha Noi", "0912345678", maMoi);
}

int main() {
    cout << "==============================================================\n";
    cout << "  HE THONG QUAN LY KHO, SAN PHAM VA DON HANG (PBL2) - HUY\n";
    cout << "==============================================================\n";
    cout << "-> Dang khoi tao du lieu he thong...\n";

    vector<KhoHang> danhSachKho;
    khoiTaoKhoHang(danhSachKho);

    QuanLySanPham qlSP(101); // Hash Table dung luong 101 bucket
    khoiTaoSanPham(qlSP);

    QuanLyDonHang qlDH;
    khoiTaoDonHang(qlDH, qlSP);

    cout << "-> Khoi tao thanh cong: 5 Kho hang, 21 San pham, 4 Don hang mau!\n";

    int luaChon = -1;
    while (luaChon != 0) {
        cout << "\n======================== MENU CHINH ========================\n";
        cout << "--- [MODULE QUAN LY KHO & SAN PHAM] ---\n";
        cout << " 1. Xem danh sach cac kho hang (KhoHang)\n";
        cout << " 2. Xem toan bo san pham trong he thong (HashTable)\n";
        cout << " 3. Tra cuu san pham theo Ma SP (HashTable - O(1))\n";
        cout << " 4. Tim kiem goi y san pham theo Ten (Trie Autocomplete - O(L))\n";
        cout << " 5. Sap xep danh sach san pham bang QuickSort (Gia, Ton kho, Ten)\n";
        cout << " 6. Them san pham moi (SanPhamThuong / SanPhamTuoiSong)\n";
        cout << " 7. Cap nhat so luong ton kho san pham (Nhap / Xuat kho)\n";
        cout << " 8. Xoa san pham (Dong bo giua Trie, MinHeap va HashTable)\n";
        cout << " 9. Kiem tra canh bao het hang (Ton kho <= nguong toi thieu)\n";
        cout << "10. Kiem tra canh bao hang tuoi song sap het han (Min-Heap)\n";
        cout << "\n--- [MODULE QUAN LY DON HANG & DUYET DON] ---\n";
        cout << "11. Dat don hang moi (Khach hang mua hang -> OrderQueue & BST)\n";
        cout << "12. Duyet don hang o dau Hang doi (Kiem tra kho, chong nghen HOL)\n";
        cout << "13. Xem Hang doi don hang dang cho duyet (Main Queue FIFO)\n";
        cout << "14. Xem Danh sach treo do thieu kho (Pending List)\n";
        cout << "15. Khoi phuc don hang tu Pending List vao lai Hang doi\n";
        cout << "16. Kiem tra va tu dong huy don hang qua han 5 phut (Pending)\n";
        cout << "17. Khach hang yeu cau huy don hang (Khi con cho duyet)\n";
        cout << "18. Tra cuu don hang theo Ma don hang (BST - O(log N))\n";
        cout << "19. Xem danh sach tat ca don hang tang dan theo ma (BST In-Order)\n";
        cout << "20. Xem lich su don hang theo Ma khach hang (Nguoi dung)\n";
        cout << "\n 0. Thoat chuong trinh\n";
        cout << "============================================================\n";
        cout << "Nhap lua chon cua ban (0 - 20): ";

        if (!(cin >> luaChon)) {
            cout << "Lua chon khong hop le! Vui long nhap so.\n";
            xoaBoDem();
            continue;
        }

        switch (luaChon) {
            case 1: {
                cout << "\n--- DANH SACH KHO HANG TRONG HE THONG (" << danhSachKho.size() << " kho) ---\n";
                for (size_t i = 0; i < danhSachKho.size(); ++i) {
                    cout << i + 1 << ". Ma kho: " << danhSachKho[i].getMaKho()
                         << " | Vi tri: " << danhSachKho[i].getViTri()
                         << " | Suc chua: " << danhSachKho[i].getSucChuaToiDa()
                         << " | SP lien ket: " << danhSachKho[i].getMaSanPham() << "\n";
                }
                break;
            }

            case 2: {
                qlSP.hienThiToanBo();
                break;
            }

            case 3: {
                cout << "\nNhap Ma san pham can tra cuu (vi du: SP01, SP12): ";
                string maSP;
                cin >> maSP;
                SanPham* sp = qlSP.timSanPhamTheoMa(maSP);
                if (sp != nullptr) {
                    cout << "\n[KET QUA TRA CUU BANG BAM O(1)]:\n";
                    sp->show();
                    cout << "\nChuoi xuat file: " << sp->xuatchuoifile() << "\n";
                } else {
                    cout << "Khong tim thay san pham voi ma: " << maSP << "\n";
                }
                break;
            }

            case 4: {
                cout << "\nNhap tu khoa hoac tien to ten san pham (vi du: 'Gao', 'Thit', 'Mi'): ";
                xoaBoDem();
                string tuKhoa;
                getline(cin, tuKhoa);

                cout << "\n[GOI Y AUTOCOMPLETE QUA CAY TIEN TO TRIE - O(L)]:\n";
                vector<string> goiY = qlSP.getCayTienTo().traCuuGoiY(tuKhoa);
                if (goiY.empty()) {
                    cout << "Khong co san pham nao phu hop voi tien to: \"" << tuKhoa << "\"\n";
                } else {
                    cout << "Tim thay " << goiY.size() << " tu khoa goi y:\n";
                    for (size_t i = 0; i < goiY.size(); ++i) {
                        cout << "  " << i + 1 << ". " << goiY[i] << "\n";
                    }

                    cout << "\nChi tiet san pham tuong ung:\n";
                    vector<SanPham*> dsSP = qlSP.timKiemTheoTen(tuKhoa);
                    for (SanPham* sp : dsSP) {
                        cout << " - [" << sp->getMaSP() << "] " << sp->getTenSP()
                             << " | Don gia: " << sp->getDonGia()
                             << " | Ton kho: " << sp->getSoLuongTonKho() << "\n";
                    }
                }
                break;
            }

            case 5: {
                cout << "\nChon tieu chi sap xep bang QuickSort:\n";
                cout << " 1. Gia tang dan\n";
                cout << " 2. Gia giam dan\n";
                cout << " 3. Ton kho tang dan\n";
                cout << " 4. Ton kho giam dan\n";
                cout << " 5. Ten san pham (A-Z)\n";
                cout << "Nhap lua chon: ";
                int tc;
                cin >> tc;

                vector<SanPham*> ds = qlSP.layDanhSachSanPham(tc);
                cout << "\n--- KET QUA SAP XEP QUICKSORT (" << ds.size() << " san pham) ---\n";
                cout << left << setw(8) << "Ma SP"
                     << setw(32) << "Ten san pham"
                     << setw(14) << "Don gia"
                     << setw(12) << "Ton kho"
                     << setw(15) << "Loai SP" << "\n";
                cout << string(80, '-') << "\n";

                for (SanPham* sp : ds) {
                    cout << left << setw(8) << sp->getMaSP()
                         << setw(32) << sp->getTenSP()
                         << setw(14) << sp->getDonGia()
                         << setw(12) << sp->getSoLuongTonKho()
                         << setw(15) << sp->getLoaiSP() << "\n";
                }
                break;
            }

            case 6: {
                cout << "\nChon loai san pham can them:\n";
                cout << " 1. San pham thuong (hang kho, bao bi, gia dung)\n";
                cout << " 2. San pham tuoi song (can bao quan lanh)\n";
                cout << "Nhap lua chon: ";
                int loai;
                cin >> loai;

                string maSP, tenSP, danhMuc, maNCC;
                double donGia;
                int soLuong, hanDung;
                cout << "Nhap Ma SP (vi du: SP22): "; cin >> maSP;
                cout << "Nhap Ten SP: "; xoaBoDem(); getline(cin, tenSP);
                cout << "Nhap Danh muc: "; getline(cin, danhMuc);
                cout << "Nhap Don gia: "; cin >> donGia;
                cout << "Nhap So luong ton kho: "; cin >> soLuong;
                cout << "Nhap Ma NCC: "; cin >> maNCC;
                cout << "Nhap Han dung (so ngay): "; cin >> hanDung;

                time_t nowTime = time(nullptr);
                bool ok = false;
                if (loai == 1) {
                    string quyCach;
                    cout << "Nhap Quy cach dong goi (vi du: Thung 24 lon, Goi 500g): ";
                    xoaBoDem(); getline(cin, quyCach);
                    ok = qlSP.themSanPham(new SanPhamThuong(maSP, tenSP, danhMuc, donGia, soLuong, maNCC, nowTime, hanDung, quyCach));
                } else if (loai == 2) {
                    double nhietDo;
                    cout << "Nhap Nhiet do bao quan (do C, vi du: -2.5 hoac 4.0): ";
                    cin >> nhietDo;
                    ok = qlSP.themSanPham(new SanPhamTuoiSong(maSP, tenSP, danhMuc, donGia, soLuong, maNCC, nowTime, hanDung, nhietDo));
                }

                if (ok) {
                    cout << "-> Them san pham thanh cong! Da dong bo vao HashTable, Trie"
                         << (loai == 2 ? " va MinHeap.\n" : ".\n");
                } else {
                    cout << "-> Them san pham that bai! (Ma SP da ton tai hoac du lieu khong hop le).\n";
                }
                break;
            }

            case 7: {
                cout << "\nNhap Ma san pham can cap nhat: ";
                string maSP; cin >> maSP;
                cout << "Nhap so luong thay doi (+ de nhap them kho, - de xuat kho): ";
                int delta; cin >> delta;

                if (qlSP.capNhatSoLuongKho(maSP, delta)) {
                    SanPham* sp = qlSP.timSanPhamTheoMa(maSP);
                    cout << "-> Cap nhat thanh cong! So luong ton kho hien tai cua "
                         << sp->getTenSP() << " la: " << sp->getSoLuongTonKho() << "\n";
                } else {
                    cout << "-> Cap nhat that bai! (Khong tim thay ma SP hoac ton kho bi am).\n";
                }
                break;
            }

            case 8: {
                cout << "\nNhap Ma san pham can xoa: ";
                string maSP; cin >> maSP;
                if (qlSP.xoaSanPham(maSP)) {
                    cout << "-> Xoa san pham thanh cong! Da go khoi Trie, MinHeap va giai phong tren HashTable.\n";
                } else {
                    cout << "-> Khong tim thay san pham can xoa!\n";
                }
                break;
            }

            case 9: {
                cout << "\nNhap nguong ton kho canh bao (mac dinh nhap 10): ";
                int nguong; cin >> nguong;
                qlSP.kiemTraCanhBaoHetHang(nguong);
                break;
            }

            case 10: {
                cout << "\nNhap so ngay toi han de canh bao (vi du nhap 2): ";
                int soNgay; cin >> soNgay;
                vector<SanPham*> sapHetHan = qlSP.kiemTraCanhBaoHetHan(soNgay);
                cout << "\n--- [CANH BAO MIN-HEAP] HANG TUOI SONG SAP HET HAN (trong " << soNgay << " ngay toi) ---\n";
                if (sapHetHan.empty()) {
                    cout << "-> An toan: Khong co mat hang tuoi song nao can ngay het han.\n";
                } else {
                    time_t hienTai = time(nullptr);
                    for (SanPham* sp : sapHetHan) {
                        double soNgayConLai = difftime(sp->ngayhethang(), hienTai) / 86400.0;
                        cout << " [!] CANH BAO: [" << sp->getMaSP() << "] " << sp->getTenSP()
                             << " | Con lai: " << fixed << setprecision(1) << soNgayConLai << " ngay"
                             << " (Ton kho: " << sp->getSoLuongTonKho() << ")\n";
                    }
                }
                break;
            }

            case 11: {
                cout << "\n--- DAT HANG MOI ---\n";
                string maND, maSP, tenKH, diaChi, sdt;
                int soLuong;
                cout << "Nhap Ma khach hang (vi du: KH01, KH02): "; cin >> maND;
                cout << "Nhap Ma san pham muon mua (vi du: SP01, SP12): "; cin >> maSP;
                cout << "Nhap So luong mua: "; cin >> soLuong;
                cout << "Nhap Ten khach hang: "; xoaBoDem(); getline(cin, tenKH);
                cout << "Nhap Dia chi giao hang: "; getline(cin, diaChi);
                cout << "Nhap So dien thoai: "; cin >> sdt;

                string maDHMoi;
                if (qlDH.datHang(maND, qlSP, maSP, soLuong, tenKH, diaChi, sdt, maDHMoi)) {
                    cout << "-> Dat hang thanh cong! Ma don hang vua tao: " << maDHMoi << "\n";
                    cout << "   Don da duoc xep vao cuoi Hang doi (OrderQueue) va Cay nhi phan (BST).\n";
                } else {
                    cout << "-> Dat hang that bai! (So luong khong hop le hoac san pham khong ton tai).\n";
                }
                break;
            }

            case 12: {
                cout << "\n--- TIEN HANH DUYET DON HANG O DAU HANG DOI ---\n";
                string thongBao;
                bool kq = qlDH.duyetDonHangTiepTheo(qlSP, thongBao);
                cout << "Ket qua: " << thongBao << "\n";
                if (!kq) {
                    cout << "(*) Chu y: Don da duoc bop tach sang Pending List de khong can tro don phia sau (Chong nghen Head-of-Line Blocking)!\n";
                }
                break;
            }

            case 13: {
                qlDH.xemHangDoi();
                break;
            }

            case 14: {
                qlDH.xemDanhSachPending();
                break;
            }

            case 15: {
                cout << "\nNhap Ma don hang can khoi phuc tu Pending List (vi du: DH003): ";
                string maDH; cin >> maDH;
                if (qlDH.phucHoiDonPending(maDH, qlSP)) {
                    cout << "-> Khoi phuc thanh cong! Kho da du hang, don " << maDH
                         << " da duoc dua tro lai Hang doi chinh (Queue) de cho duyet.\n";
                } else {
                    cout << "-> Khoi phuc that bai! (Kho van chua du hang de dap ung hoac khong tim thay ma don trong Pending List).\n";
                }
                break;
            }

            case 16: {
                cout << "\n--- KIEM TRA VA TU DONG HUY DON HANG TREO QUA 5 PHUT ---\n";
                qlDH.kiemTraVaHuyDonTimeout();
                cout << "-> Da kiem tra xong Pending List!\n";
                break;
            }

            case 17: {
                cout << "\nNhap Ma don hang muon huy: ";
                string maDH; cin >> maDH;
                cout << "Nhap Ma khach hang (neu Admin huy thi de trong hoac go KH tuong ung): ";
                xoaBoDem();
                string maND; getline(cin, maND);

                if (qlDH.huyDonHang(maDH, maND)) {
                    cout << "-> Huy don hang " << maDH << " thanh cong (Don dang o trang thai cho duyet)!\n";
                } else {
                    cout << "-> Huy don hang that bai! (Don khong ton tai, sai ma khach hang, hoac da xuat kho van chuyen).\n";
                }
                break;
            }

            case 18: {
                cout << "\nNhap Ma don hang can tim tren cay BST (vi du: DH001, DH002): ";
                string maDH; cin >> maDH;
                DonHang* dh = qlDH.timDonHangTheoMa(maDH);
                if (dh != nullptr) {
                    cout << "\n[KET QUA TRA CUU NHI PHAN CAY BST - O(log N)]:\n";
                    dh->show();
                } else {
                    cout << "-> Khong tim thay don hang: " << maDH << " tren cay BST!\n";
                }
                break;
            }

            case 19: {
                qlDH.xemDanhSachDonHangBST();
                break;
            }

            case 20: {
                cout << "\nNhap Ma nguoi dung can xem lich su (vi du: KH01, KH02, KH03): ";
                string maND; cin >> maND;
                qlDH.xemLichSuDonHang(maND);
                break;
            }

            case 0: {
                cout << "\nTam biet! Cam on ban da su dung chuong trinh test he thong PBL2!\n";
                break;
            }

            default: {
                cout << "Lua chon khong hop le! Vui long nhap lai tu 0 den 20.\n";
                break;
            }
        }
    }

    return 0;
}

