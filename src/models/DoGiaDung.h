#ifndef DOGIADUNG_H
#define DOGIADUNG_H

#include "SanPham.h"
#include <iostream>
#include <string>

using namespace std;

class DoGiaDung : public SanPham {
    private:
        string chatLieu; // Chat lieu che tao: Inox 304, Gom su, Thuy tinh chiu nhiet, Nhua ABS...

    protected:
        void inra(ostream&) const override;
        void docvao(istream&) override;

    public:
        DoGiaDung();
        DoGiaDung(const DoGiaDung&);
        DoGiaDung(const string& maSP, const string& tenSP,
                  const string& danhMuc, const double& donGia,
                  const int& soLuongTonKho, const string& maNCC,
                  const time_t& ngaySX, const string& chatLieu = "Inox / Hop kim");
        ~DoGiaDung() override = default;

        const string& getChatLieu() const;
        void setChatLieu(const string&);

        bool canbaoquanlanh() const override;
        string xuatchuoifile() const override;
};

#endif // DOGIADUNG_H

