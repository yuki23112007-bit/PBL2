#include "GaussianNaiveBayes.h"
#include <cmath>
#include <algorithm>
#include <limits>

static const double PI = 3.14159265358979323846;
static const double EPSILON = 1e-9; // Tranh chia cho 0

GaussianNaiveBayes::GaussianNaiveBayes()
    : soLop(0), soDacTrung(0), soLuongMau(0), daHuanLuyen(false) {}

GaussianNaiveBayes::~GaussianNaiveBayes() {}

void GaussianNaiveBayes::fit(const std::vector<std::vector<double>>& X, const std::vector<int>& y) {
    if (X.empty() || y.empty() || X.size() != y.size()) {
        daHuanLuyen = false;
        return;
    }

    soLuongMau = static_cast<int>(X.size());
    soDacTrung = static_cast<int>(X[0].size());

    // Xac dinh so lop phan loai: max(y) + 1
    int maxClass = 0;
    for (int label : y) {
        if (label > maxClass) maxClass = label;
    }
    soLop = maxClass + 1;

    // Khoi tao mang dem va mang tich luy
    std::vector<int> counts(soLop, 0);
    priors.assign(soLop, 0.0);
    means.assign(soLop, std::vector<double>(soDacTrung, 0.0));
    vars.assign(soLop, std::vector<double>(soDacTrung, 0.0));

    // 1. Tinh tong de tinh trung binh mu
    for (int i = 0; i < soLuongMau; ++i) {
        int c = y[i];
        if (c < 0 || c >= soLop) continue;
        counts[c]++;
        for (int j = 0; j < soDacTrung; ++j) {
            means[c][j] += X[i][j];
        }
    }

    for (int c = 0; c < soLop; ++c) {
        priors[c] = static_cast<double>(counts[c]) / static_cast<double>(soLuongMau);
        if (counts[c] > 0) {
            for (int j = 0; j < soDacTrung; ++j) {
                means[c][j] /= static_cast<double>(counts[c]);
            }
        }
    }

    // 2. Tinh phuong sai sigma^2
    for (int i = 0; i < soLuongMau; ++i) {
        int c = y[i];
        if (c < 0 || c >= soLop) continue;
        for (int j = 0; j < soDacTrung; ++j) {
            double diff = X[i][j] - means[c][j];
            vars[c][j] += diff * diff;
        }
    }

    for (int c = 0; c < soLop; ++c) {
        for (int j = 0; j < soDacTrung; ++j) {
            if (counts[c] > 1) {
                vars[c][j] = (vars[c][j] / static_cast<double>(counts[c])) + EPSILON;
            } else {
                vars[c][j] = 1.0; // Fallback khi khong du mau
            }
        }
    }

    daHuanLuyen = true;
}

std::vector<double> GaussianNaiveBayes::predictProba(const std::vector<double>& x) const {
    if (!daHuanLuyen || soLop == 0) {
        return std::vector<double>();
    }

    std::vector<double> logProbs(soLop, 0.0);

    for (int c = 0; c < soLop; ++c) {
        if (priors[c] <= 0.0) {
            logProbs[c] = -1e9;
            continue;
        }

        double logPrior = std::log(priors[c]);
        double logLikelihood = 0.0;

        for (int j = 0; j < soDacTrung; ++j) {
            double mu = means[c][j];
            double var = vars[c][j];
            double diff = x[j] - mu;

            // ln N(x ; mu, var) = -0.5 * ln(2*pi*var) - (x - mu)^2 / (2*var)
            double logPdf = -0.5 * std::log(2.0 * PI * var) - ((diff * diff) / (2.0 * var));
            logLikelihood += logPdf;
        }

        logProbs[c] = logPrior + logLikelihood;
    }

    // Chuan hoa bang Softmax / Log-Sum-Exp de chong tran so
    double maxLog = -std::numeric_limits<double>::infinity();
    for (double lp : logProbs) {
        if (lp > maxLog) maxLog = lp;
    }

    std::vector<double> expWeights(soLop, 0.0);
    double sumExp = 0.0;
    for (int c = 0; c < soLop; ++c) {
        expWeights[c] = std::exp(logProbs[c] - maxLog);
        sumExp += expWeights[c];
    }

    std::vector<double> probas(soLop, 0.0);
    if (sumExp > 0.0) {
        for (int c = 0; c < soLop; ++c) {
            probas[c] = expWeights[c] / sumExp;
        }
    }

    return probas;
}
