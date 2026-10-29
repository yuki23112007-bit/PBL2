#ifndef THUCPHAMTUOISONG_H
#define THUCPHAMTUOISONG_H

#include "ThucPham.h"
#include <iostream>
#include <string>

using namespace std;

class ThucPhamTuoiSong : public ThucPham {
    private:
        double nhietDoBaoQuan; // Nhiet do bao quan (do C)

    protected:
        void inra(ostream&) const override;
        void docvao(istream&) override;

    public:
        ThucPhamTuoiSong();
        ThucPhamTuoiSong(const ThucPhamTuoiSong&);
        ThucPhamTuoiSong(const string& maSP, const string& tenSP,
                         const string& danhMuc, const double& donGia,
                         const int& soLuongTonKho, const string& maNCC,
                         const time_t& ngaySX, const int& hanDungSoNgay,
                         const double& nhietDo);
        ~ThucPhamTuoiSong() override = default;

        double getNhietDoBaoQuan() const;
        void setNhietDoBaoQuan(const double&);

        bool canbaoquanlanh() const override;
        string xuatchuoifile() const override;
};

#endif // THUCPHAMTUOISONG_H

