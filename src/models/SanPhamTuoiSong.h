#ifndef SANPHAMTUOISONG_H
#define SANPHAMTUOISONG_H

#include "SanPham.h"
#include <iostream>
#include <string>

using namespace std;

class SanPhamTuoiSong : public SanPham {
    private:
        double nhietDoBaoQuan;

    protected:
        void inra(ostream&) const override;
        void docvao(istream&) override;

    public:
        SanPhamTuoiSong();
        SanPhamTuoiSong(const SanPhamTuoiSong&);
        SanPhamTuoiSong(const string& maSP, const string& tenSP,
                        const string& danhMuc, const double& donGia,
                        const int& soLuongTonKho, const string& maNCC,
                        const time_t& ngaySX, const int& hanDungSoNgay,
                        const double& nhietDo);
        ~SanPhamTuoiSong() override = default;

        double getNhietDoBaoQuan() const;
        void setNhietDoBaoQuan(const double&);

        bool canbaoquanlanh() const override;
        string xuatchuoifile() const override;
};

#endif // SANPHAMTUOISONG_H
