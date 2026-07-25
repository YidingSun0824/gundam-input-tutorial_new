# Response Function Plotting

This folder contains ROOT C++ macros for inspecting the response functions stored in the mock input dataset.

## drawResponseFunctions.C

`drawResponseFunctions.C` reads event-by-event systematic response functions from the `events` TTree in:

```text
example/advanced/inputs/datasets/mydataset.root
```

The response functions are stored in the following branches:

- `par1_TGraph`
- `par2_TGraph`
- `par3_TGraph`

From the repository root, run:

```bash
root -l 'plotting/drawResponseFunctions.C("example/advanced/inputs/datasets/mydataset.root",5)'
```

The second argument specifies the number of events to plot. In this example, the macro draws the response functions for the first five events.

The macro creates the multi-page PDF:

```text
response_functions.pdf
```

The response functions for `par1` and `par2` are smooth. The response function for `par3` is intentionally non-smooth so that different interpolation methods can be tested and compared.

---

## compareGraphSpline.C

Compares two interpolation strategies for the per-event response-function knots:

- **TGraph linear** — straight-line segments between knots (ROOT `TGraph::Eval`, the default in GUNDAM's `Graph` dial type)
- **TSpline3 cubic** — natural cubic spline through the same knots (ROOT `TSpline3`, the base for GUNDAM's `Spline` dial types)

Both macros read from the same dataset and tree as `drawResponseFunctions.C`.

### Task-3 mode — one pad per dial

```bash
root -l -q 'plotting/compareGraphSpline.C(12)'
```

Produces a three-panel canvas (par1 | par2 | par3), each panel showing:
- Knot markers (raw stored points)
- Dashed line: TGraph linear interpolation
- Solid line: TSpline3 cubic interpolation

Output: `output/plots/compareGraphSpline_event<N>.pdf`

### Task-4 mode — multi-pad comparison + supplementary overlay

```bash
root -l -q 'plotting/compareGraphSpline.C(12, true)'
```

Produces **two** output files:

**Primary result** — three-panel canvas, same layout as Task-3:

```
output/plots/compareGraphSpline_overlay_event<N>.pdf
```

Each panel shows TGraph (linear, dashed) vs TSpline3 (cubic, solid) for one dial.
This is the main Graph-vs-Spline interpolation comparison for Task 4.

**Supplementary view** — single canvas with all three dials' TSpline3 curves overlaid:

```
output/plots/compareGraphSpline_alloverlay_event<N>.pdf
```

This figure shows only TSpline3 curves (no linear curves) for a quick side-by-side
shape comparison of the three dials.
It is **not** the Graph-vs-Spline interpolation comparison — use the primary result above for that.

Replace `12` with any valid event number (0 - 999999).
All commands must be run from the **repository root**.