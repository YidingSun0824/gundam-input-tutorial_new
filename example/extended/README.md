# Extended GUNDAM Example

Builds on the Basic tutorial. Demonstrates multiple concurrent selections,
multiple free normalization parameters, spline response dials, correlated
normalization parameters driven by a covariance matrix, and a comparison of
interpolation methods (Spline natural, Spline not-a-knot, Graph) on the same
non-smooth response function.

All commands are run from the **repository root**.

---

## Files in this package

| File | Purpose |
|---|---|
| `selection_1d.txt` | 1D binning in pmu (6 bins) — used by Extended01–Extended07 |
| `selection_2d.txt` | 2D binning in pmu × pp (33 bins) — used by Extended01–Extended07 |
| `selection_3d.txt` | 3D binning in enu × pmu × pp (8 bins) — used by Extended01–Extended07 |
| `parameters.txt` | Free normalization parameter binning (reaction × pmu, 10 bins) — used by Extended02–Extended03, Extended05–Extended07 |
| `correlated_parameters.txt` | Parameter binning for the correlated set (enu, 11 bins) — used by Extended04–Extended07 |
| `covarianceFile.root` | Covariance matrix ROOT file for the correlated parameter set — used by Extended04–Extended07 |

---

### Extended01 — Multiple selections

```
gundamFitter -d -c example/extended/E01_multiple_selections.yaml
```

Defines three concurrent samples, each with a different binning dimensionality:

- Selection 1 uses `selection_1d.txt` (1D: pmu)
- Selection 2 uses `selection_2d.txt` (2D: pmu × pp)
- Selection 3 uses `selection_3d.txt` (3D: enu × pmu × pp)

No parameters are defined. Run a dry run using the option `-d`: with zero
free parameters there is nothing for the minimizer to fit, so a full fit
would error out.

### Extended02 — Multiple free normalization parameters

```
gundamFitter -c example/extended/E02_multiple_normparam.yaml
```

Adds 10 free normalization parameters generated from `parameters.txt`.
Each row in `parameters.txt` defines one parameter bin in reaction × pmu space.

### Extended03 — Spline response example

```
gundamFitter -c example/extended/E03_response.yaml
```

Extends step 05 with a Spline dial (`dialType: Spline`, `options: not-a-knot`)
reading the per-event response function from the `par1_TGraph` branch.

### Extended04 — Correlated normalization parameters

```
gundamFitter -c example/extended/E04_correlated_normparam.yaml
```

Replaces free normalization parameters with a covariance-matrix-driven set.
The parameter binning is read from `correlated_parameters.txt` (enu, 11 bins)
and the covariance matrix is read from `covarianceFile.root`.

### Extended05 — Spline (natural) interpolation

```
gundamFitter -c example/extended/E05_spline_natural.yaml
```

Adds a `Dummy Systematics` parameter set with a Spline dial (`dialType: Spline`,
`options: natural`) reading the intentionally non-smooth response function
from the `par3_TGraph` branch, on top of the free and correlated normalization
parameters from Extended02–Extended04.

### Extended06 — Spline (not-a-knot) interpolation

```
gundamFitter -c example/extended/E06_spline_not_a_knot.yaml
```

Same as Extended05, but the `par3_TGraph` dial uses `options: not-a-knot`
instead of `natural`.

### Extended07 — Graph interpolation

```
gundamFitter -c example/extended/E07_graph_interpolation.yaml
```

Same as Extended05, but the `par3_TGraph` dial uses `dialType: Graph`
(piecewise-linear interpolation, no `options`) instead of a Spline.

Together, Extended05–Extended07 compare post-fit uncertainty across three
interpolation methods applied to the same intentionally non-smooth
systematic (`par3_TGraph`). See `mockDatasetGen/plotting/plot_interp_comparison.C`
for a script that plots the comparison after all three fits have been run.

---

## Input organization

See [Input Organization: Flat vs. Modular](../../README.md#input-organization-flat-vs-modular) in the root README.
