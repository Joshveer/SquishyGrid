# SquishyGrid

Small C++ simulation experiments (various configurations live in subfolders like `2-point/`, `3-point/`, `4-point/`, `monotone/`, etc.). Most folders contain:

- `*.cpp`: C++ source for a specific experiment
- binaries like `ball`, `grad_all_angle`, `trace_path`, etc.
- `output_*.txt`: generated output data
- `plot_*.py`: quick plotting scripts
- `*.png`: saved plots/images

## Requirements

- C++ compiler (examples below use `g++`)
- Python 3 (only needed for plotting)

## Build (examples)

Compile a program from the repo root:

```bash
g++ -O2 -std=c++17 ball.cpp -o ball
g++ -O2 -std=c++17 grad_all_angle.cpp -o grad_all_angle
g++ -O2 -std=c++17 diff_n.cpp -o diff_n
```

Compile inside a subfolder:

```bash
cd 2-point
g++ -O2 -std=c++17 ball.cpp -o ball
```

## Run

Run the compiled binary in the same directory:

```bash
./ball
```

Outputs are typically written to `output_*.txt` in the same folder.

## Plotting

From the folder containing the plot script:

```bash
python3 plot_ball.py
python3 plot_path.py
python3 plot_width.py
```

Plots are usually saved as `*.png` in the same directory.
