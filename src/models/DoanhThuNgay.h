#ifndef DOANHTHUNGAY_H
#define DOANHTHUNGAY_H

#include <iostream>
#include <string>

using namespace std;

class DoanhThuNgay {
private:
    string ngayThongKe; // Định dạng YYYY-MM-DD (Khóa chính tra cứu trên HashTable)
    double tongDoanhThu; // Tổng doanh thu tích lũy trong ngày
    int tongSoDonHang;   // Tổng số đơn hàng hoàn thành trong ngày

public:
    DoanhThuNgay();
    DoanhThuNgay(const string& ngay, double doanhThu = 0.0, int soDon = 0);
    virtual ~DoanhThuNgay() = default;

    // Getter
    const string& getNgayThongKe() const;
    double getTongDoanhThu() const;
    int getTongSoDonHang() const;

    // Setter
    void setNgayThongKe(const string& ngay);
    void setTongDoanhThu(double doanhThu);
    void setTongSoDonHang(int soDon);

    // Nghiệp vụ (Phân hệ 1.5, 1.7)
    void congDonDoanhThu(double giaTriDonHang);
    void congDoanhThu(double giaTriDonHang); // Alias thuận tiện cho congDonDoanhThu

    // Hiển thị & xuất dữ liệu
    virtual void hienThiThongTin() const;
    void hienThiBaoCao() const { hienThiThongTin(); }
    void show() const { hienThiThongTin(); }
    string xuatChuoiFile() const;

    // Toán tử luồng
    friend ostream& operator<<(ostream& os, const DoanhThuNgay& dt);
    friend istream& operator>>(istream& is, DoanhThuNgay& dt);
};

#endif // DOANHTHUNGAY_H
