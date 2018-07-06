#include <fstream>
#include <ceres/ceres.h>

using namespace std;
//using namespace corecvs;

// f(x,y) = (1-x)^2 + 100(y - x^2)^2;
class Rosenbrock : public ceres::FirstOrderFunction {
 public:
  virtual ~Rosenbrock() {}
  virtual bool Evaluate(const double* parameters,
                        double* cost,
                        double* gradient) const
  {
      const double x = parameters[0];
      const double y = parameters[1];
      cost[0] = (1.0 - x) * (1.0 - x) + 100.0 * (y - x * x) * (y - x * x);

      if (gradient != NULL) {
          gradient[0] = -2.0 * (1.0 - x) - 200.0 * (y - x * x) * 2.0 * x;
          gradient[1] = 200.0 * (y - x * x);
      }
      return true;
  }
  virtual int NumParameters() const { return 2; }
};

using namespace ceres;

int main(int argc, char** argv) {

    double parameters[2] = {-1.2, 1.0};

    GradientProblemSolver::Options options;
    options.minimizer_progress_to_stdout = true;

    GradientProblemSolver::Summary summary;
    GradientProblem problem(new Rosenbrock());

    ceres::Solve(options, problem, parameters, &summary);
    std::cout << summary.FullReport() << "\n";
    std::cout << "Initial x: " << -1.2 << " y: " << 1.0 << "\n";
    std::cout << "Final   x: " << parameters[0]
              << " y: " << parameters[1] << "\n";
    return 0;
}
