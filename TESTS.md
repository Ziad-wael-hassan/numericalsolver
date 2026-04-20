# TESTS

## Automated Smoke Test

Build and run the solver-only smoke test:

```bash
g++ -std=c++17 -I NumericalSolver \
  NumericalSolver/tests/solver_smoke.cpp \
  NumericalSolver/core/parser/Tokenizer.cpp \
  NumericalSolver/core/parser/Parser.cpp \
  NumericalSolver/core/parser/Evaluator.cpp \
  NumericalSolver/core/validation/Validator.cpp \
  NumericalSolver/core/solvers/SolverCommon.cpp \
  NumericalSolver/core/solvers/BisectionSolver.cpp \
  NumericalSolver/core/solvers/FalsePositionSolver.cpp \
  NumericalSolver/core/solvers/NewtonRaphsonSolver.cpp \
  NumericalSolver/core/solvers/FixedPointSolver.cpp \
  NumericalSolver/core/solvers/SecantSolver.cpp \
  NumericalSolver/core/solvers/GaussianSolver.cpp \
  NumericalSolver/core/solvers/LUSolver.cpp \
  NumericalSolver/core/solvers/PALUSolver.cpp \
  NumericalSolver/core/solvers/GaussJordanSolver.cpp \
  NumericalSolver/core/solvers/CramerSolver.cpp \
  NumericalSolver/core/solvers/GoldenSectionSolver.cpp \
  NumericalSolver/core/solvers/ConjugateGradientSolver.cpp \
  NumericalSolver/core/solvers/SolverFactory.cpp \
  NumericalSolver/utils/NumericalDiff.cpp \
  -o /tmp/solver_smoke

/tmp/solver_smoke
```

Expected output:

```text
solver_smoke: ok
```

## Covered Smoke Cases

- Bisection: `x^3 - x - 2` on `[1, 2]` expects `~1.52138`
- Gaussian Elimination: classic `3x3` system expects `[2, 3, -1]`
- Golden-Section Search: `x^2 - 4x + 4` on `[0, 4]` expects `x=2`, `f(x)=0`
- Conjugate Gradient: `A=[[4,1],[1,3]]`, `b=[1,2]` expects `[1/11, 7/11]`

## Manual GUI Checks

1. Open each method from the sidebar and confirm the input panel changes shape correctly.
2. Click `Load Example` and verify fields populate with a valid sample problem.
3. Run each chapter sample and confirm:
   - Solution tab shows the final scalar/vector result
   - Iteration tab shows rows and supports CSV export
   - Graph tab renders a chart when Qt Charts is installed, or a fallback notice when it is not
4. Trigger validation failures:
   - Bisection with no sign change
   - Newton-Raphson with derivative near zero
   - Cramer's Rule on a singular matrix
   - Conjugate Gradient with a non-symmetric matrix
5. Toggle dark mode, restart the app, and confirm the preference persists.
