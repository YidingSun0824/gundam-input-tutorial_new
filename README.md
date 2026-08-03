# Input Tutorial for GUNDAM

## Tutorial structure

The tutorial is organized into three progressive packages under `example/`:

```text
example/
├── basic/      Steps 01–03: dataset loading, selection, one normalization parameter
├── extended/   Steps 04–07: multiple selections, spline dials, correlated parameters
└── advanced/   Full modular configuration (simpleFit/ sub-tree)

mockDatasetGen/ Mock dataset generator (buildInputRootFile.C)
plotting/       Response-function inspection macros
```

The entry point for the full modular run is `example/advanced/simpleFit/mainConfig.yaml`.

The old deep structure (`inputs/fitter/likelihood/...`) has been moved to `_archive/` for reference.

---

## Building the input files

Generate the mock ROOT dataset before running any fitter step.
Run the build macro from the repository root, then move the output to the expected location:

```bash
root mockDatasetGen/buildInputRootFile.C
mv mydataset.root example/advanced/simpleFit/inputs/datasets/mydataset.root
```

The expected output from the build macro:

```
Processing mockDatasetGen/buildInputRootFile.C...
File writen in ./example/advanced/simpleFit/inputs/datasets/mydataset.root:/
```

---

## Input Organization: Flat vs. Modular

The `basic/` and `extended/` packages keep their small `.txt` binning and selection
tables right next to each config file.
Every step is fully standalone and can be run in isolation without touching any
other package — the only shared resource is the ROOT dataset in
`example/advanced/simpleFit/inputs/datasets/`.

The `advanced/` package mirrors how a real large-scale analysis is organized:
configuration is split across a dedicated `inputs/` sub-tree (datasets, samples,
parameters, plots), with a single `mainConfig.yaml` entry point that references each
sub-config by path.
This separation makes each concern independently readable and editable.

---

## Running GUNDAM

### Basic examples (steps 01–03)

```bash
gundamFitter -c example/basic/B01_load_dataset.yaml
gundamFitter -d -c example/basic/B02_selection.yaml
gundamFitter -c example/basic/B03_normparam.yaml
gundamFitter -c example/basic/B04_datafit.yaml

```

### Extended examples (steps 04–07)

```bash
gundamFitter -d -c example/extended/E01_multiple_selections.yaml
gundamFitter -c example/extended/E02_multiple_normparam.yaml
gundamFitter -c example/extended/E03_response.yaml
gundamFitter -c example/extended/E04_correlated_normparam.yaml
```

### Full modular run

```bash
gundamFitter -a -c example/advanced/simpleFit/mainConfig.yaml -t 4
```

---

## Dial types reference

### Common options (all dial types)

| Option | Description |
|---|---|
| `dialType` | One of: `Normalization`, `Spline`, `Graph`, `Surface`, `Tabulated`, `Kriged`, `Formula` |
| `dialSubType` | Variant within a type (e.g. `MonotonicSpline`, `Bilinear`) |
| `isEnabled` | `true` / `false` — skip this dial set without removing it |
| `applyOnDataSets` | List of dataset names; `["*"]` matches all |
| `applyCondition` | Single ROOT formula string; dial only applies when true |
| `applyConditions` | List of ROOT formula strings (all must be true) |
| `minDialResponse` | Hard floor on the computed weight |
| `maxDialResponse` | Hard ceiling on the computed weight |
| `binningFilePath` | Apply dial only within specific bins |
| `useMirrorDial` | Reflect the dial shape across an edge |
| `mirrorLowEdge` | Lower mirror boundary |
| `mirrorHighEdge` | Upper mirror boundary |
| `allowDialExtrapolation` | Extrapolate beyond the defined parameter range |
| `printDialsSummary` | Print dial summary table at startup |
| `definitionRange` | Parameter range over which the dial is defined |
| `mirrorDefinitionRange` | Definition range used for the mirrored side |
| `dialInputList` | Explicit list of dial input objects (alternative to `dialLeafName`) |
| `dialsTreePath` | Path inside a ROOT file to the tree holding dial objects |

### Type-specific options

#### Normalization
| Option | Description |
|---|---|
| `parametersBinningPath` | Optional `.txt` binning file for bin-by-bin normalisation |

#### Spline
| Option | Description |
|---|---|
| `dialLeafName` | Branch name of `TClonesArray<TGraph>` in the event tree |
| `dialsFilePath` | External ROOT file holding spline objects |
| `dialsList` | Explicit list of spline object names inside `dialsFilePath` |
| `dialSubType` | `Uniform` \| `General` \| `Monotonic` \| `Akima` |

#### Graph
| Option | Description |
|---|---|
| `dialLeafName` | Branch name of `TClonesArray<TGraph>` in the event tree |
| `dialsFilePath` | External ROOT file holding graph objects |
| `dialsList` | Explicit list of graph object names inside `dialsFilePath` |

#### Surface
| Option | Description |
|---|---|
| `dialLeafName` | Branch name identifying which surface bin each event falls in |
| `dialsFilePath` | External ROOT file holding the 2-D surface |
| `dialsList` | Explicit list of surface object names |
| `dialSubType` | `Bilinear` \| `Bicubic` |

#### Tabulated
| Option | Description |
|---|---|
| `tableConfig` | Config block pointing to the pre-computed weight table |

#### Kriged
| Option | Description |
|---|---|
| `tableConfig` | Config block pointing to the kriging data source |

#### Formula
| Option | Description |
|---|---|
| *(formula string)* | ROOT `TFormula` expression evaluated per event; set via the `formula` key. Not GPU-accelerated — avoid in large-scale fits. |
