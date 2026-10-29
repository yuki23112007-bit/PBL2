#ifndef SANPHAMTHUONG_H
#define SANPHAMTHUONG_H

#include "SanPham.h"
#include <iostream>
#include <string>

using namespace std;

class SanPhamThuong : public SanPham {
    private:
        string quyCachDongGoi;

    protected:
        void inra(ostream&) const override;
        void docvao(istream&) override;

    public:
        SanPhamThuong();
        SanPhamThuong(const SanPhamThuong&);
        SanPhamThuong(const string& maSP, const string& tenSP,
                      const string& danhMuc, const double& donGia,
                      const int& soLuongTonKho, const string& maNCC,
                      const time_t& ngaySX, const int& hanDungSoNgay,
                      const string& quyCach);
        ~SanPhamThuong() override = default;

        const string& getQuyCachDongGoi() const;
        void setQuyCachDongGoi(const string&);

        // Alias
        const string& getQuyCach() const { return getQuyCachDongGoi(); }
        void setQuyCach(const string& v) { setQuyCachDongGoi(v); }

        bool canbaoquanlanh() const override;
        string xuatchuoifile() const override;
};

#endif // SANPHAMTHUONG_H
