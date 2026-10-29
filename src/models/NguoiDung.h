#include <string>
#include <vector>

using namespace std;

// Khai báo chuyển tiếp (Forward Declaration)
class DonHang;

class NguoiDung : public TaiKhoan {
private:
    string hoTen;
    string soDienThoai;
    string diaChi;
    vector<DonHang*> danhSachDonHangCaNhan; // Chứa con trỏ tới các đơn hàng của khách (O(k) tra cứu)

public:
    NguoiDung();
    NguoiDung(const string& id, const string& username, const string& passwordHash,
              const string& hoTen, const string& soDienThoai, const string& diaChi);

    // Theo mục 5.1 README: NguoiDung chỉ giữ con trỏ mượn từ QuanLyDonHang, không giải phóng bộ nhớ
    ~NguoiDung() override = default;

    // Getter
    const string& getHoTen() const;
    const string& getSoDienThoai() const;
    const string& getDiaChi() const;
    const vector<DonHang*>& getDanhSachDonHang() const;

    // Setter
    void setHoTen(const string& v);
    void setSoDienThoai(const string& v);
    void setDiaChi(const string& v);

    // Nghiệp vụ theo mục 5.1 README
    void themDonHang(DonHang* dh);

    void show() const override;
    string xuatChuoiFile() const override;
};
