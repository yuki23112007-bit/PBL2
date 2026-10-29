#ifndef THUCPHAMTHUONG_H
#define THUCPHAMTHUONG_H

#include "ThucPham.h"
#include <iostream>
#include <string>

using namespace std;

class ThucPhamThuong : public ThucPham {
    private:
        string quyCachDongGoi;

    protected:
        void inra(ostream&) const override;
        void docvao(istream&) override;

    public:
        ThucPhamThuong();
        ThucPhamThuong(const ThucPhamThuong&);
        ThucPhamThuong(const string& maSP, const string& tenSP,
                       const string& danhMuc, const double& donGia,
                       const int& soLuongTonKho, const string& maNCC,
                       const time_t& ngaySX, const int& hanDungSoNgay,
                       const string& quyCach);
        ~ThucPhamThuong() override = default;

        const string& getQuyCachDongGoi() const;
        void setQuyCachDongGoi(const string&);

        // Bi danh phu hop tai lieu
        const string& getQuyCach() const { return getQuyCachDongGoi(); }
        void setQuyCach(const string& v) { setQuyCachDongGoi(v); }

        bool canbaoquanlanh() const override;
        string xuatchuoifile() const override;
};

#endif // THUCPHAMTHUONG_H

