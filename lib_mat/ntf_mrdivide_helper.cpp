//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: ntf_mrdivide_helper.cpp
//
// MATLAB Coder version            : 5.0
// C/C++ source code generated on  : 24-Jun-2020 22:09:55
//

// Include Files
#include "ntf_mrdivide_helper.h"
#include "ntf_ERANataf.h"
#include "ntf_inataf.h"
#include "ntf_inataf_rtwutil.h"
#include "ntf_pdf.h"
#include "ntf_xnrm2.h"
#include "ntf_xzgeqp3.h"
#include "rt_nonfinite.h"
#include <cmath>
#include <cstring>

// Function Definitions

//
// Arguments    : coder::array<double, 2U> *A
//                const coder::array<double, 2U> *B
// Return Type  : void
//
void ntf_b_mrdiv(coder::array<double, 2U> &A, const coder::array<double, 2U> &B)
{
  int maxmn;
  coder::array<double, 2U> b_A;
  coder::array<double, 2U> b_B;
  int minmana;
  coder::array<int, 2U> jpvt;
  coder::array<double, 1U> tau;
  int rankR;
  double tol;
  coder::array<double, 2U> Y;
  if ((A.size(0) == 0) || (B.size(0) == 0)) {
    int nb;
    maxmn = A.size(0);
    A.set_size(A.size(0), B.size(0));
    nb = B.size(0);
    for (int i = 0; i < nb; i++) {
      for (int i1 = 0; i1 < maxmn; i1++) {
        A[i1 + A.size(0) * i] = 0.0;
      }
    }
  } else if (B.size(0) == B.size(1)) {
    int n;
    int nb;
    int i;
    int i1;
    int k;
    int ldap1;
    int u1;
    int j;
    int b_nb;
    int mn;
    int b_i;
    n = B.size(1);
    b_A.set_size(B.size(0), B.size(1));
    nb = B.size(0) * B.size(1);
    for (i = 0; i < nb; i++) {
      b_A[i] = B[i];
    }

    minmana = B.size(1);
    jpvt.set_size(1, B.size(1));
    jpvt[0] = 1;
    maxmn = 1;
    for (k = 2; k <= minmana; k++) {
      maxmn++;
      jpvt[k - 1] = maxmn;
    }

    ldap1 = B.size(1);
    maxmn = B.size(1) - 1;
    u1 = B.size(1);
    if (maxmn < u1) {
      u1 = maxmn;
    }

    for (j = 0; j < u1; j++) {
      int jj;
      b_nb = n - j;
      nb = j * (n + 1);
      jj = j * (ldap1 + 1);
      mn = nb + 2;
      if (b_nb < 1) {
        minmana = -1;
      } else {
        minmana = 0;
        if (b_nb > 1) {
          rankR = nb;
          tol = std::abs(b_A[jj]);
          for (k = 2; k <= b_nb; k++) {
            double s;
            rankR++;
            s = std::abs(b_A[rankR]);
            if (s > tol) {
              minmana = k - 1;
              tol = s;
            }
          }
        }
      }

      if (b_A[jj + minmana] != 0.0) {
        if (minmana != 0) {
          maxmn = j + minmana;
          jpvt[j] = maxmn + 1;
          rankR = j;
          for (k = 0; k < n; k++) {
            tol = b_A[rankR];
            b_A[rankR] = b_A[maxmn];
            b_A[maxmn] = tol;
            rankR += n;
            maxmn += n;
          }
        }

        i = jj + b_nb;
        for (b_i = mn; b_i <= i; b_i++) {
          b_A[b_i - 1] = b_A[b_i - 1] / b_A[jj];
        }
      }

      maxmn = nb + n;
      minmana = jj + ldap1;
      for (nb = 0; nb <= b_nb - 2; nb++) {
        tol = b_A[maxmn];
        if (b_A[maxmn] != 0.0) {
          rankR = jj + 1;
          i = minmana + 2;
          i1 = b_nb + minmana;
          for (mn = i; mn <= i1; mn++) {
            b_A[mn - 1] = b_A[mn - 1] + b_A[rankR] * -tol;
            rankR++;
          }
        }

        maxmn += n;
        minmana += n;
      }
    }

    b_nb = A.size(0);
    if (A.size(0) != 0) {
      for (j = 0; j < n; j++) {
        maxmn = b_nb * j - 1;
        minmana = n * j;
        for (k = 0; k < j; k++) {
          nb = b_nb * k;
          i = k + minmana;
          if (b_A[i] != 0.0) {
            for (b_i = 0; b_i < b_nb; b_i++) {
              mn = b_i + 1;
              i1 = mn + maxmn;
              A[i1] = A[i1] - b_A[i] * A[(mn + nb) - 1];
            }
          }
        }

        tol = 1.0 / b_A[j + minmana];
        for (b_i = 0; b_i < b_nb; b_i++) {
          i = (b_i + maxmn) + 1;
          A[i] = tol * A[i];
        }
      }
    }

    if (A.size(0) != 0) {
      for (j = n; j >= 1; j--) {
        maxmn = b_nb * (j - 1) - 1;
        minmana = n * (j - 1) - 1;
        i = j + 1;
        for (k = i; k <= n; k++) {
          nb = b_nb * (k - 1);
          i1 = k + minmana;
          if (b_A[i1] != 0.0) {
            for (b_i = 0; b_i < b_nb; b_i++) {
              mn = b_i + 1;
              rankR = mn + maxmn;
              A[rankR] = A[rankR] - b_A[i1] * A[(mn + nb) - 1];
            }
          }
        }
      }
    }

    i = B.size(1) - 1;
    for (j = i; j >= 1; j--) {
      i1 = jpvt[j - 1];
      if (i1 != j) {
        for (b_i = 0; b_i < b_nb; b_i++) {
          tol = A[b_i + A.size(0) * (j - 1)];
          A[b_i + A.size(0) * (j - 1)] = A[b_i + A.size(0) * (i1 - 1)];
          A[b_i + A.size(0) * (i1 - 1)] = tol;
        }
      }
    }
  } else {
    int n;
    int nb;
    int i;
    int i1;
    int k;
    int u1;
    int j;
    int b_nb;
    int mn;
    int b_i;
    b_A.set_size(B.size(1), B.size(0));
    nb = B.size(0);
    for (i = 0; i < nb; i++) {
      minmana = B.size(1);
      for (i1 = 0; i1 < minmana; i1++) {
        b_A[i1 + b_A.size(0) * i] = B[i + B.size(0) * i1];
      }
    }

    b_B.set_size(A.size(1), A.size(0));
    nb = A.size(0);
    for (i = 0; i < nb; i++) {
      minmana = A.size(1);
      for (i1 = 0; i1 < minmana; i1++) {
        b_B[i1 + b_B.size(0) * i] = A[i + A.size(0) * i1];
      }
    }

    n = b_A.size(1) - 1;
    maxmn = b_A.size(0);
    minmana = b_A.size(1);
    if (maxmn < minmana) {
      minmana = maxmn;
    }

    tau.set_size(minmana);
    for (i = 0; i < minmana; i++) {
      tau[i] = 0.0;
    }

    maxmn = b_A.size(0);
    u1 = b_A.size(1);
    if (maxmn < u1) {
      u1 = maxmn;
    }

    if (u1 < 1) {
      jpvt.set_size(1, b_A.size(1));
      nb = b_A.size(1);
      for (i = 0; i < nb; i++) {
        jpvt[i] = 0;
      }

      for (j = 0; j <= n; j++) {
        jpvt[j] = j + 1;
      }
    } else {
      jpvt.set_size(1, b_A.size(1));
      nb = b_A.size(1);
      for (i = 0; i < nb; i++) {
        jpvt[i] = 0;
      }

      for (k = 0; k <= n; k++) {
        jpvt[k] = k + 1;
      }

      ntf_qrpf(b_A, b_A.size(0), b_A.size(1), tau, jpvt);
    }

    rankR = 0;
    if (b_A.size(0) < b_A.size(1)) {
      minmana = b_A.size(0);
      maxmn = b_A.size(1);
    } else {
      minmana = b_A.size(1);
      maxmn = b_A.size(0);
    }

    if (minmana > 0) {
      tol = 2.2204460492503131E-15 * static_cast<double>(maxmn);
      if (1.4901161193847656E-8 < tol) {
        tol = 1.4901161193847656E-8;
      }

      tol *= std::abs(b_A[0]);
      while ((rankR < minmana) && (!(std::abs(b_A[rankR + b_A.size(0) * rankR]) <=
               tol))) {
        rankR++;
      }
    }

    b_nb = b_B.size(1);
    Y.set_size(b_A.size(1), b_B.size(1));
    nb = b_A.size(1) * b_B.size(1);
    for (i = 0; i < nb; i++) {
      Y[i] = 0.0;
    }

    minmana = b_A.size(0);
    nb = b_B.size(1);
    maxmn = b_A.size(0);
    mn = b_A.size(1);
    if (maxmn < mn) {
      mn = maxmn;
    }

    for (j = 0; j < mn; j++) {
      if (tau[j] != 0.0) {
        for (k = 0; k < nb; k++) {
          tol = b_B[j + b_B.size(0) * k];
          i = j + 2;
          for (b_i = i; b_i <= minmana; b_i++) {
            tol += b_A[(b_i + b_A.size(0) * j) - 1] * b_B[(b_i + b_B.size(0) * k)
              - 1];
          }

          tol *= tau[j];
          if (tol != 0.0) {
            b_B[j + b_B.size(0) * k] = b_B[j + b_B.size(0) * k] - tol;
            i = j + 2;
            for (b_i = i; b_i <= minmana; b_i++) {
              b_B[(b_i + b_B.size(0) * k) - 1] = b_B[(b_i + b_B.size(0) * k) - 1]
                - b_A[(b_i + b_A.size(0) * j) - 1] * tol;
            }
          }
        }
      }
    }

    for (k = 0; k < b_nb; k++) {
      for (b_i = 0; b_i < rankR; b_i++) {
        Y[(jpvt[b_i] + Y.size(0) * k) - 1] = b_B[b_i + b_B.size(0) * k];
      }

      for (j = rankR; j >= 1; j--) {
        i = jpvt[j - 1];
        Y[(i + Y.size(0) * k) - 1] = Y[(i + Y.size(0) * k) - 1] / b_A[(j +
          b_A.size(0) * (j - 1)) - 1];
        for (b_i = 0; b_i <= j - 2; b_i++) {
          Y[(jpvt[b_i] + Y.size(0) * k) - 1] = Y[(jpvt[b_i] + Y.size(0) * k) - 1]
            - Y[(jpvt[j - 1] + Y.size(0) * k) - 1] * b_A[b_i + b_A.size(0) * (j
            - 1)];
        }
      }
    }

    A.set_size(Y.size(1), Y.size(0));
    nb = Y.size(0);
    for (i = 0; i < nb; i++) {
      minmana = Y.size(1);
      for (i1 = 0; i1 < minmana; i1++) {
        A[i1 + A.size(0) * i] = Y[i + Y.size(0) * i1];
      }
    }
  }
}

//
// Arguments    : const double A_data[]
//                const int A_size[2]
//                const double B_data[]
//                const int B_size[2]
// Return Type  : double
//
double ntf_mrdiv(const double A_data[], const int A_size[2], const double
                 B_data[], const int B_size[2])
{
  double Y;
  double b_A_data[4];
  double b_B_data[4];
  double atmp;
  double wj;
  double beta1;
  coder::array<double, 1U> x;
  if (1 == B_size[1]) {
    Y = A_data[0] / B_data[0];
  } else {
    int A_size_idx_0;
    int rankR;
    int m;
    double tau_data_idx_0;
    A_size_idx_0 = B_size[1];
    rankR = B_size[1];
    if (0 <= rankR - 1) {
      std::memcpy(&b_A_data[0], &B_data[0], rankR * sizeof(double));
    }

    rankR = A_size[1];
    if (0 <= rankR - 1) {
      std::memcpy(&b_B_data[0], &A_data[0], rankR * sizeof(double));
    }

    m = B_size[1];
    atmp = b_A_data[0];
    tau_data_idx_0 = 0.0;
    wj = ntf_c_xnrm2(B_size[1] - 1, b_A_data);
    if (wj != 0.0) {
      beta1 = ntf_rt_hypotd_snf(b_A_data[0], wj);
      if (b_A_data[0] >= 0.0) {
        beta1 = -beta1;
      }

      if (std::abs(beta1) < 1.0020841800044864E-292) {
        int knt;
        int i;
        knt = -1;
        do {
          knt++;
          x.set_size(A_size_idx_0);
          for (i = 0; i < A_size_idx_0; i++) {
            x[i] = b_A_data[i];
          }

          for (rankR = 2; rankR <= m; rankR++) {
            x[rankR - 1] = 9.9792015476736E+291 * x[rankR - 1];
          }

          A_size_idx_0 = x.size(0);
          rankR = x.size(0);
          for (i = 0; i < rankR; i++) {
            b_A_data[i] = x[i];
          }

          beta1 *= 9.9792015476736E+291;
          atmp *= 9.9792015476736E+291;
        } while (!(std::abs(beta1) >= 1.0020841800044864E-292));

        beta1 = ntf_rt_hypotd_snf(atmp, ntf_c_xnrm2(B_size[1] - 1, (double *)
          x.data()));
        if (atmp >= 0.0) {
          beta1 = -beta1;
        }

        tau_data_idx_0 = (beta1 - atmp) / beta1;
        wj = 1.0 / (atmp - beta1);
        for (rankR = 2; rankR <= m; rankR++) {
          x[rankR - 1] = wj * x[rankR - 1];
        }

        A_size_idx_0 = x.size(0);
        rankR = x.size(0);
        for (i = 0; i < rankR; i++) {
          b_A_data[i] = x[i];
        }

        for (rankR = 0; rankR <= knt; rankR++) {
          beta1 *= 1.0020841800044864E-292;
        }

        atmp = beta1;
      } else {
        int i;
        tau_data_idx_0 = (beta1 - b_A_data[0]) / beta1;
        wj = 1.0 / (b_A_data[0] - beta1);
        x.set_size(B_size[1]);
        for (i = 0; i < A_size_idx_0; i++) {
          x[i] = b_A_data[i];
        }

        for (rankR = 2; rankR <= A_size_idx_0; rankR++) {
          x[rankR - 1] = wj * x[rankR - 1];
        }

        A_size_idx_0 = x.size(0);
        rankR = x.size(0);
        for (i = 0; i < rankR; i++) {
          b_A_data[i] = x[i];
        }

        atmp = beta1;
      }
    }

    b_A_data[0] = atmp;
    rankR = 0;
    wj = std::abs(atmp);
    if (!(wj <= 2.2204460492503131E-15 * static_cast<double>(A_size_idx_0) * wj))
    {
      rankR = 1;
    }

    Y = 0.0;
    if (tau_data_idx_0 != 0.0) {
      wj = b_B_data[0];
      for (m = 2; m <= A_size_idx_0; m++) {
        wj += b_A_data[m - 1] * b_B_data[m - 1];
      }

      wj *= tau_data_idx_0;
      if (wj != 0.0) {
        b_B_data[0] -= wj;
        for (m = 2; m <= A_size_idx_0; m++) {
          b_B_data[m - 1] -= b_A_data[m - 1] * wj;
        }
      }
    }

    for (m = 0; m < rankR; m++) {
      Y = b_B_data[0];
    }

    for (m = rankR; m >= 1; m--) {
      Y /= atmp;
    }
  }

  return Y;
}

//
// File trailer for ntf_mrdivide_helper.cpp
//
// [EOF]
//