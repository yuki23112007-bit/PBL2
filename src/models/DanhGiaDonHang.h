#ifndef DANHGIADONHANG_H
#define DANHGIADONHANG_H

#include <iostream>
#include <string>
#include <ctime>

using namespace std;

// Khai báo chuyển tiếp (Forward Declarations)
class DonHang;
class NguoiDung;

class DanhGiaDonHang {
private:
    string maDanhGia;       // Khóa chính (PK, ví dụ: "DG001")
    string maDonHang;       // Khóa ngoại trỏ tới DonHang (FK)
    string maNguoiDung;     // Khóa ngoại trỏ tới khách hàng đánh giá (FK)
    int soSao;              // Số sao đánh giá: 1 đến 5 sao
    string noiDung;         // Ý kiến, nhận xét hoặc nội dung khiếu nại của khách
    time_t thoiGianDanhGia; // Thời điểm gửi phản hồi / đánh giá
    bool laKhieuNai;        // true: Khiếu nại sự cố (giữ đơn, không tự hoàn thành); false: Đánh giá bình thường

public:
    DanhGiaDonHang();
    DanhGiaDonHang(const string& maDG, const string& maDH, const string& maND,
                   int sao, const string& noiDung, bool khieuNai = false,
                   time_t thoiGian = 0);
    virtual ~DanhGiaDonHang() = default;

    // Getter
    const string& getMaDanhGia() const;
    const string& getMaDonHang() const;
    const string& getMaNguoiDung() const;
    int getSoSao() const;
    const string& getNoiDung() const;
    time_t getThoiGianDanhGia() const;
    bool getLaKhieuNai() const;

    // Setter
    void setMaDanhGia(const string& ma);
    void setMaDonHang(const string& maDH);
    void setMaNguoiDung(const string& maND);
    void setSoSao(int sao);
    void setNoiDung(const string& nd);
    void setThoiGianDanhGia(time_t tg);
    void setLaKhieuNai(bool khieuNai);

    // Nghiệp vụ
    bool isKhieuNai() const;
    bool isHaiLong() const;
    bool coYKien() const;

    // Hiển thị & xuất chuỗi file
    virtual void hienThiThongTin() const;
    void show() const { hienThiThongTin(); }
    string xuatChuoiFile() const;

    // Đa năng hóa toán tử
    friend ostream& operator<<(ostream& os, const DanhGiaDonHang& dg);
    friend istream& operator>>(istream& is, DanhGiaDonHang& dg);
};

#endif // DANHGIADONHANG_H

