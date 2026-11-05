#include "ThuatToan.h"

using namespace std;

int ThuatToan::phanHoach(vector<double>& arr, int low, int high, bool tangDan) {
    double pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        bool dieuKien = tangDan ? (arr[j] <= pivot) : (arr[j] >= pivot);
        if (dieuKien) {
            i++;
            double tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
        }
    }
    double tmp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = tmp;
    return i + 1;
}

void ThuatToan::quickSort(vector<double>& arr, int low, int high, bool tangDan) {
    if (low < high) {
        int p = phanHoach(arr, low, high, tangDan);
        quickSort(arr, low, p - 1, tangDan);
        quickSort(arr, p + 1, high, tangDan);
    }
}

int ThuatToan::timKiemNhiPhan(const vector<double>& arr, double giaTri) {
    int low = 0;
    int high = (int)arr.size() - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == giaTri) return mid;
        if (arr[mid] < giaTri) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

