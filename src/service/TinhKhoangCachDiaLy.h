#ifndef TINHKHOANGCACHDIALY_H
#define TINHKHOANGCACHDIALY_H

#include <string>
#include <vector>

using namespace std;

struct ToaDoThanhPho {
    string tenChuan;
    double lat;
    double lon;
    vector<string> tuKhoa;
};

// Lop chuyen trach tinh khoang cach dia ly (SRP):
// - Nap bang toa do tinh/thanh tu file data/cities.txt
// - Chuan hoa chuoi UTF-8 tieng Viet & nhan dien tinh/thanh pho tu dia chi
// - Tinh khoang cach duong bo (km) theo cong thuc Haversine
class TinhKhoangCachDiaLy {
private:
    static vector<ToaDoThanhPho> dsThanhPho;
    static bool daNapFile;

public:
    static bool napTuFile(const string& duongDan);
    static string chuanHoaChuThuongUtf8(const string& s);
    static const vector<ToaDoThanhPho>& danhSachThanhPho();
    static const ToaDoThanhPho* timThanhPhoTuDiaChi(const string& diaChi);
    static double tinhKhoangCachKm(const ToaDoThanhPho& a, const ToaDoThanhPho& b);
    static double tinhKhoangCachGiuaDiaChi(const string& diaChiA, const string& diaChiB, double macDinhKm = 50.0);
};

#endif // TINHKHOANGCACHDIALY_H
