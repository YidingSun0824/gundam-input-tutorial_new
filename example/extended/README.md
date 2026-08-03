# Extended GUNDAM Example

Builds on the Basic tutorial. Demonstrates multiple concurrent selections,
multiple free normalization parameters, spline response dials, and correlated
normalization parameters driven by a covariance matrix.

All commands are run from the **repository root**.

---

## Files in this package

| File | Purpose |
|---|---|
| `selection_1d.txt` | 1D binning in pmu (6 bins) — used by Extended01–Extended04 |
| `selection_2d.txt` | 2D binning in pmu × pp (33 bins) — used by Extended01–Extended04 |
| `selection_3d.txt` | 3D binning in enu × pmu × pp (8 bins) — used by Extended01–Extended04 |
| `parameters.txt` | Free normalization parameter binning (reaction × pmu, 10 bins) — used by Extended02–Extended03 |
| `correlated_parameters.txt` | Parameter binning for the correlated set (enu, 11 bins) — used by Extended04 |
| `covarianceFile.root` | Covariance matrix ROOT file for the correlated parameter set — used by Extended04 |

---

### Extended01 — Multiple selections

```
gundamFitter -c example/extended/E01_multiple_selections.yaml
```

Defines three concurrent samples, each with a different binning dimensionality:

- Selection 1 uses `selection_1d.txt` (1D: pmu)
- Selection 2 uses `selection_2d.txt` (2D: pmu × pp)
- Selection 3 uses `selection_3d.txt` (3D: enu × pmu × pp)

No parameters are defined.

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

Extends step 05 with a Spline dial (`dialType: Spline`, `dialSubType: not-a-knot`)
reading the per-event response function from the `par1_TGraph` branch.

### Extended04 — Correlated normalization parameters

```
gundamFitter -c example/extended/E04_correlated_normparam.yaml
```

Replaces free normalization parameters with a covariance-matrix-driven set.
The parameter binning is read from `correlated_parameters.txt` (enu, 11 bins)
and the covariance matrix is read from `covarianceFile.root`.

---

## Input organization

See [Input Organization: Flat vs. Modular](../../README.md#input-organization-flat-vs-modular) in the root README.
