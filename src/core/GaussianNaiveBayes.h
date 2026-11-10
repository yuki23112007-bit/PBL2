#ifndef GAUSSIAN_NAIVE_BAYES_H
#define GAUSSIAN_NAIVE_BAYES_H

#include "IBoPhanLoai.h"
#include <vector>
#include <string>

// ============================================================================
// Cai dat thuan C++ mo hinh Gaussian Naive Bayes ke thua IBoPhanLoai
// ============================================================================
class GaussianNaiveBayes : public IBoPhanLoai {
private:
    int soLop;                               // So luong lop phan loai (K)
    int soDacTrung;                          // So luong dac trung (d)
    int soLuongMau;                          // So luong mau huan luyen
    std::vector<double> priors;              // P(y = c)
    std::vector<std::vector<double>> means;  // mu[c][j]: gia tri trung binh
    std::vector<std::vector<double>> vars;   // sigma^2[c][j]: phuong sai (+ epsilon)
    bool daHuanLuyen;

public:
    GaussianNaiveBayes();
    virtual ~GaussianNaiveBayes() override;

    virtual void fit(const std::vector<std::vector<double>>& X, const std::vector<int>& y) override;
    virtual std::vector<double> predictProba(const std::vector<double>& x) const override;

    bool isDaHuanLuyen() const { return daHuanLuyen; }
    int getSoLop() const { return soLop; }
    int getSoDacTrung() const { return soDacTrung; }
    int getSoLuongMau() const { return soLuongMau; }
};

#endif // GAUSSIAN_NAIVE_BAYES_H
