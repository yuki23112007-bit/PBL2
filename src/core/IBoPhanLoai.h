#ifndef I_BO_PHAN_LOAI_H
#define I_BO_PHAN_LOAI_H

#include <vector>

// ============================================================================
// Interface truu tuong cho Bo Phan Loai Machine Learning (Nguyen ly D trong SOLID)
// ============================================================================
class IBoPhanLoai {
public:
    virtual ~IBoPhanLoai() {}

    // Huan luyen mo hinh voi tap dac trung X (n mau x d dac trung) va nhan y (n nhan so nguyen)
    virtual void fit(const std::vector<std::vector<double>>& X, const std::vector<int>& y) = 0;

    // Du doan xac suat cua tung lop cho mot vector dac trung x
    // Tra ve vector xac suat [P(c_0), P(c_1), ..., P(c_{k-1})] voi tong bang 1.0
    virtual std::vector<double> predictProba(const std::vector<double>& x) const = 0;
};

#endif // I_BO_PHAN_LOAI_H
