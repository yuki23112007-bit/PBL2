#ifndef VANCHUYENFORMATTER_H
#define VANCHUYENFORMATTER_H

#include <iostream>
#include <string>
#include "../models/VanChuyen.h"

using namespace std;

// Lop chuyen trach dinh dang, hien thi va xuat chuoi file cho VanChuyen (SRP)
class VanChuyenFormatter {
public:
    static void hienThiThongTin(const VanChuyen& vc, ostream& os = cout);
    static string xuatChuoiFile(const VanChuyen& vc);
};

#endif // VANCHUYENFORMATTER_H
