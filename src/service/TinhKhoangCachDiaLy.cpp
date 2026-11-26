#include "TinhKhoangCachDiaLy.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <utility>

using namespace std;

vector<ToaDoThanhPho> TinhKhoangCachDiaLy::dsThanhPho;
bool TinhKhoangCachDiaLy::daNapFile = false;

static string trimChuoi(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool TinhKhoangCachDiaLy::napTuFile(const string& duongDan) {
    ifstream f(duongDan);
    if (!f.is_open()) return false;

    vector<ToaDoThanhPho> dsMoi;
    string dong;
    while (getline(f, dong)) {
        dong = trimChuoi(dong);
        if (dong.empty() || dong[0] == '#') continue;

        stringstream ss(dong);
        string tenChuan, latStr, lonStr, tuKhoaStr;
        if (!getline(ss, tenChuan, '|')) continue;
        if (!getline(ss, latStr, '|')) continue;
        if (!getline(ss, lonStr, '|')) continue;
        getline(ss, tuKhoaStr);

        ToaDoThanhPho tp;
        tp.tenChuan = trimChuoi(tenChuan);
        try {
            tp.lat = stod(trimChuoi(latStr));
            tp.lon = stod(trimChuoi(lonStr));
        } catch (...) {
            continue;
        }

        stringstream kwss(tuKhoaStr);
        string kw;
        while (getline(kwss, kw, ',')) {
            kw = chuanHoaChuThuongUtf8(trimChuoi(kw));
            if (!kw.empty()) tp.tuKhoa.push_back(kw);
        }
        if (tp.tuKhoa.empty() && !tp.tenChuan.empty()) {
            tp.tuKhoa.push_back(chuanHoaChuThuongUtf8(tp.tenChuan));
        }

        if (!tp.tenChuan.empty()) {
            dsMoi.push_back(move(tp));
        }
    }

    dsThanhPho = move(dsMoi);
    daNapFile = true;
    return !dsThanhPho.empty();
}

string TinhKhoangCachDiaLy::chuanHoaChuThuongUtf8(const string& s) {
    string out = s;
    for (char& c : out) {
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    }
    static const pair<const char*, const char*> thayThe[] = {
        {"À","à"},{"Á","á"},{"Ả","ả"},{"Ã","ã"},{"Ạ","ạ"},
        {"Ă","ă"},{"Ằ","ằ"},{"Ắ","ắ"},{"Ẳ","ẳ"},{"Ẵ","ẵ"},{"Ặ","ặ"},
        {"Â","â"},{"Ầ","ầ"},{"Ấ","ấ"},{"Ẩ","ẩ"},{"Ẫ","ẫ"},{"Ậ","ậ"},
        {"Đ","đ"},
        {"È","è"},{"É","é"},{"Ẻ","ẻ"},{"Ẽ","ẽ"},{"Ẹ","ẹ"},
        {"Ê","ê"},{"Ề","ề"},{"Ế","ế"},{"Ể","ể"},{"Ễ","ễ"},{"Ệ","ệ"},
        {"Ì","ì"},{"Í","í"},{"Ỉ","ỉ"},{"Ĩ","ĩ"},{"Ị","ị"},
        {"Ò","ò"},{"Ó","ó"},{"Ỏ","ỏ"},{"Õ","õ"},{"Ọ","ọ"},
        {"Ô","ô"},{"Ồ","ồ"},{"Ố","ố"},{"Ổ","ổ"},{"Ỗ","ỗ"},{"Ộ","ộ"},
        {"Ơ","ơ"},{"Ờ","ờ"},{"Ớ","ớ"},{"Ở","ở"},{"Ỡ","ỡ"},{"Ợ","ợ"},
        {"Ù","ù"},{"Ú","ú"},{"Ủ","ủ"},{"Ũ","ũ"},{"Ụ","ụ"},
        {"Ư","ư"},{"Ừ","ừ"},{"Ứ","ứ"},{"Ử","ử"},{"Ữ","ữ"},{"Ự","ự"},
        {"Ỳ","ỳ"},{"Ý","ý"},{"Ỷ","ỷ"},{"Ỹ","ỹ"},{"Ỵ","ỵ"}
    };
    for (const auto& kv : thayThe) {
        string from = kv.first;
        string to = kv.second;
        size_t pos = 0;
        while ((pos = out.find(from, pos)) != string::npos) {
            out.replace(pos, from.size(), to);
            pos += to.size();
        }
    }
    return out;
}

const vector<ToaDoThanhPho>& TinhKhoangCachDiaLy::danhSachThanhPho() {
    if (!daNapFile) {
        napTuFile("data/cities.txt");
    }
    return dsThanhPho;
}

const ToaDoThanhPho* TinhKhoangCachDiaLy::timThanhPhoTuDiaChi(const string& diaChi) {
    string norm = chuanHoaChuThuongUtf8(diaChi);
    for (const auto& tp : danhSachThanhPho()) {
        for (const auto& kw : tp.tuKhoa) {
            if (norm.find(kw) != string::npos) return &tp;
        }
    }
    return nullptr;
}

double TinhKhoangCachDiaLy::tinhKhoangCachKm(const ToaDoThanhPho& a, const ToaDoThanhPho& b) {
    if (a.tenChuan == b.tenChuan) return 2.5; // Noi thanh cung thanh pho
    const double R = 6371.0;
    const double PI = 3.14159265358979323846;
    double dLat = (b.lat - a.lat) * PI / 180.0;
    double dLon = (b.lon - a.lon) * PI / 180.0;
    double sinLat = sin(dLat / 2.0);
    double sinLon = sin(dLon / 2.0);
    double h = sinLat * sinLat + cos(a.lat * PI / 180.0) * cos(b.lat * PI / 180.0) * sinLon * sinLon;
    double c = 2.0 * atan2(sqrt(h), sqrt(1.0 - h));
    double km = R * c * 1.25; // He so duong bo ~1.25x duong chim bay
    return round(km * 10.0) / 10.0;
}

double TinhKhoangCachDiaLy::tinhKhoangCachGiuaDiaChi(const string& diaChiA, const string& diaChiB, double macDinhKm) {
    const ToaDoThanhPho* tpA = timThanhPhoTuDiaChi(diaChiA);
    const ToaDoThanhPho* tpB = timThanhPhoTuDiaChi(diaChiB);
    if (!tpA || !tpB) return macDinhKm;
    return tinhKhoangCachKm(*tpA, *tpB);
}
