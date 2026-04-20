# NumericalSolver

Qt-based desktop application for numerical analysis methods from the `plan.md` specification. The project is organized into a headless solver core and a GUI shell so the numerical methods remain independently testable.

## Included Scope

- Chapter 1 root-finding methods: Bisection, False Position, Newton-Raphson, Fixed-Point Iteration, Secant
- Chapter 2 linear-system methods: Gaussian Elimination, LU, PA=LU, Gauss-Jordan, Cramer's Rule
- Chapter 3 optimization methods: Golden-Section Search, Conjugate Gradient
- Handwritten expression tokenizer/parser/evaluator with whitelisted syntax
- Qt widgets for method selection, method-specific input panels, solution summary, iteration table export, and graph area
- Dark-mode toggle with `QSettings` persistence

## Build

Prerequisites:

- C++17 compiler
- CMake 3.21+
- Qt 6 Widgets and Concurrent
- Qt Charts optional: when installed, graph rendering uses Qt Charts; otherwise the app still builds and the graph tab shows a fallback notice

Build commands:

```bash
cmake -S NumericalSolver -B NumericalSolver/build
cmake --build NumericalSolver/build
./NumericalSolver/build/NumericalSolver
```

## Project Layout

```text
NumericalSolver/
├── CMakeLists.txt
├── core/
│   ├── parser/
│   ├── solvers/
│   └── validation/
├── gui/
│   ├── output/
│   ├── panels/
│   └── widgets/
├── tests/
└── utils/
```

## Verification

- `tests/solver_smoke.cpp` exercises representative methods from each chapter without Qt dependencies
- The GUI layer was syntax-checked against the locally available Qt 6 Widgets and Concurrent headers
- A full `cmake` configure/build could not be run in this environment because `cmake` is not installed

## Notes

- Expression input is length-limited and validated before parsing
- The solver layer throws typed exceptions; the GUI catches and logs failures
- Screenshots are not included in this repository scaffold because the container does not provide a runnable desktop session
