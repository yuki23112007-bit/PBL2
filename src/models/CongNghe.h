#ifndef CONGNGHE_H
#define CONGNGHE_H

#include "SanPham.h"
#include <iostream>
#include <string>

using namespace std;

class CongNghe : public SanPham {
    private:
        int thoiGianBaoHanh;      // Thoi gian bao hanh (so thang, vi du: 12, 24)
        bool canBaoQuanCanThan;   // Canh bao hang de vo, can bao quan chong soc/chong am

    protected:
        void inra(ostream&) const override;
        void docvao(istream&) override;

    public:
        CongNghe();
        CongNghe(const CongNghe&);
        CongNghe(const string& maSP, const string& tenSP,
                 const string& danhMuc, const double& donGia,
                 const int& soLuongTonKho, const string& maNCC,
                 const time_t& ngaySX, const int& baoHanh = 12,
                 const bool& canThan = true);
        ~CongNghe() override = default;

        int getThoiGianBaoHanh() const;
        void setThoiGianBaoHanh(const int&);

        bool getCanBaoQuanCanThan() const;
        void setCanBaoQuanCanThan(const bool&);

        // Bi danh
        int getBaoHanh() const { return getThoiGianBaoHanh(); }
        void setBaoHanh(int v) { setThoiGianBaoHanh(v); }
        bool isCanBaoQuanCanThan() const { return getCanBaoQuanCanThan(); }

        bool canbaoquanlanh() const override;
        string xuatchuoifile() const override;
};

#endif // CONGNGHE_H

