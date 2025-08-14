#include "dde.h"
#include "graph.h"

#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

constexpr unsigned int MAX_ITER = 100;
constexpr unsigned int N_PT = 10;
// Interior point width
constexpr unsigned int INT_N_PT = N_PT - 2;

template <unsigned int N> using Vec = std::array<double, N>;
template <unsigned int N> using Grid = std::array<Vec<N>, N>;
using Domain = std::array<double, 2>;

std::random_device rd;
std::default_random_engine generator(rd());
std::uniform_real_distribution<double> distribution(0.0, 1.0);
double theexp(double x) { return std::exp(x); }

template <unsigned int N>
void pp_matrix(Grid<N> &matrix, int width = 7, int precision = 4) {
  for (const auto &row : matrix) {
    for (auto val : row) {
      std::cout << std::setw(width) << std::fixed
                << std::setprecision(precision) << val << " ";
    }
    std::cout << std::endl;
  }
}

template <unsigned int N>
void eval_residual(Grid<N> &y, Grid<N> &res, double lambda, Domain omega) {
  unsigned int n_points = N;
  unsigned int n_intervals = n_points - 1;

  double h_squared_times_lambda =
      lambda * std::pow(omega[1] - omega[0] / n_intervals, 2);

  for (int i = 1; i < n_points - 1; i++) {
    for (int j = 1; j < n_points - 1; j++) {
      res[i][j] = y[i + 1][j] + y[i - 1][j] + y[i][j + 1] + y[i][j - 1] +
                  (-4) * y[i][j] +
                  (-1) * h_squared_times_lambda * theexp(y[i][j]);
    }
  }
}

template <unsigned int N> double dot(Vec<N> &x, Vec<N> &y) {
  double n = 0.0;

  for (int i = 0; i < N; i++) {
    n += x[i] * y[i];
  }

  return n;
}
template <unsigned int N> double norm(Vec<N> x) {
  return std::pow(dot<N>(x, x), 0.5);
}

int to_vec_idx(int i, int j, int N = N_PT) { return N * i + j; }

std::array<int, 2> from_vec_idx(int vec_idx, int N = N_PT) {
  int j = vec_idx % N;
  int i = (vec_idx - j) / N;

  return {i, j};
}

template <unsigned int N> double power_iter(Grid<N> &A, int n_iter = 100) {
  Vec<N> b{0}, b_tmp;
  for (int i = 0; i < N; i++)
    b[i] = distribution(generator);

  double lambda_max = 0.0;
  int iter = 0;
  do {
    b_tmp = b;
    for (int i = 0; i < N; i++)
      b[i] = dot<N>(A[i], b_tmp);

    lambda_max = norm<N>(b);
    for (int i = 0; i < N; i++)
      b[i] /= lambda_max;
  } while (++iter < n_iter);

  for (int i = 0; i < N; i++)
    b[i] = dot<N>(A[i], b_tmp);

  return dot<N>(b_tmp, b);
}

template <unsigned int N> Vec<N> richardson(Grid<N> &A, Vec<N> &b) {
  Vec<N> x{0}, x_tmp;
  for (int i = 0; i < N; i++)
    x[i] = distribution(generator);

  double w = 1.0 / power_iter<N>(A);
  unsigned int iter = 0;
  do {
    x_tmp = x;
    for (int i = 0; i < N; i++) {
      x[i] += w * (b[i] - dot<N>(A[i], x_tmp));
    }
  } while (++iter < 500);

  return x;
}

int main() {
  /*
   * We assume a square domain, i.e.
   * Domain d = (a, b) -> mathematical domain is d * d
   */
  Domain omega{0, 1};

  // Solution and residual
  Grid<N_PT> y{0}, res{0};

  // Jacobian only for the interior points due to Dirichlet BC
  Grid<INT_N_PT * INT_N_PT> J{0};

  for (int i = 0; i < N_PT; i++) {
    for (int j = 0; j < N_PT; j++) {

      if (i == N_PT - 1) {
        // Boundary condition at x = 1
        y[i][j] = 1.0;
      } else if (i == 0 || j == 0 || j == N_PT - 1) {
        // Boundary condition at other boundaries
        y[i][j] = 0.0;
      } else {
        // Random interior initialization
        y[i][j] = distribution(generator);
      }
    }
  }

  unsigned int iter = 0;
  double solution_norm = 10;
  do {
    // Jacobian accumulation
    for (int i = 1; i < N_PT - 1; i++) {
      for (int j = 1; j < N_PT - 1; j++) {

        for (int l = 1; l < N_PT - 1; l++) {
          for (int m = 1; m < N_PT - 1; m++) {
            dde::var(&y[l][m], "y", to_vec_idx(l, m));
          }
        }

        dde::start();
        eval_residual<N_PT>(y, res, 0.5, omega);
        dde::stop();
        dde::output(&res[i][j], "r");
        dde::dump_graph();

        Graph dag("/tmp/prog.gr");
        dag.order_graph(dag.root);
        dag.root->der = 1.0;
        dag.eval_adjoints();

        // position in the Jacobian matrix of r_ij
        int row_idx = to_vec_idx(i - 1, j - 1, INT_N_PT);
        for (int l = 1; l < N_PT - 1; l++) {
          for (int m = 1; m < N_PT - 1; m++) {
            int k = to_vec_idx(l, m);
            std::string uuid = "y" + std::to_string(k);

            if (dag.nodes.count(uuid) == 0)
              continue;

            int col_idx = to_vec_idx(l - 1, m - 1, INT_N_PT);
            J[row_idx][col_idx] = dag.nodes[uuid]->der;
          }
        }
      }
    }
    // Newton step
    // delta = x_k+1 - x_k
    Vec<INT_N_PT * INT_N_PT> neg_res;
    for (int l = 1; l < N_PT - 1; l++) {
      for (int m = 1; m < N_PT - 1; m++) {
        int k = to_vec_idx(l - 1, m - 1, INT_N_PT);
        neg_res[k] = -res[l][m];
      }
    }
    Vec<INT_N_PT *INT_N_PT> delta = richardson<INT_N_PT * INT_N_PT>(J, neg_res);

    for (int l = 1; l < N_PT - 1; l++) {
      for (int m = 1; m < N_PT - 1; m++) {
        y[l][m] += delta[to_vec_idx(l - 1, m - 1, INT_N_PT)];
      }
    }
    solution_norm = norm<INT_N_PT * INT_N_PT>(delta);
    std::cout << "Error @ iter " << (iter + 1) << " is " << solution_norm
              << std::endl;
  } while (++iter < MAX_ITER && solution_norm > 1e-6);

  pp_matrix<N_PT>(res);
  std::cout << std::endl;
  pp_matrix<N_PT>(y);
}
