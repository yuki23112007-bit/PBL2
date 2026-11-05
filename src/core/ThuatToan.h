#ifndef THUATTOAN_H
#define THUATTOAN_H

#include <vector>

using namespace std;

// Lop tien ich thuat toan tu cai dat (README muc 10.1: khong dung std::sort, std::find...)
class ThuatToan {
public:
    // QuickSort cho mang so thuc, tangDan = true: tang dan
    static void quickSort(vector<double>& arr, int low, int high, bool tangDan = true);

    // Tim kiem nhi phan tren mang so thuc da sap xep tang dan, tra ve chi so hoac -1
    static int timKiemNhiPhan(const vector<double>& arr, double giaTri);

private:
    static int phanHoach(vector<double>& arr, int low, int high, bool tangDan);
};

#endif // THUATTOAN_H

