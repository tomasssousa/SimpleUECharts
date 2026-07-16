# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

- Organized the repository structure based on the roadmap.
- Added the initial Unreal Engine plugin and module skeleton.
- Completed the Phase 2 shared data model with a reusable `FChartDataPoint`
  contract for both chart widget APIs.
- Documented the Phase 2 milestone verification and shared dataset usage in the
  README.
- Completed the Phase 3 `UBarChartWidget` wrapper with explicit UMG palette
  categorization for the editor.
- Documented the Phase 3 milestone verification for the bar chart UMG widget.
- Implemented the Phase 4 minimal `SBarChart` rendering path for one or more
  bars in native Slate.
- Documented the Phase 4 milestone verification for the Slate bar chart widget.
- Implemented the Phase 5 bar chart scaling logic based on the maximum data
  value.
- Added configurable `BarSpacing` and `ChartPadding` synchronization from
  `UBarChartWidget` into `SBarChart`.
- Documented the Phase 5 milestone verification for scaled vertical bars.
- Implemented Phase 6 presentation options for `SBarChart`, including labels,
  values, Y axis, and optional grid lines.
- Exposed Phase 6 bar chart presentation toggles on `UBarChartWidget`.
- Documented the Phase 6 milestone verification for bar chart UI usability.
- Completed the Phase 7 `UPieChartWidget` UMG wrapper with shared data input,
  Blueprint API, Slate synchronization, and explicit UMG palette categorization.
- Documented the Phase 7 milestone verification for the pie chart UMG widget.
- Implemented the Phase 8 pie-chart calculation pass in `SPieChart` for total
  value aggregation, per-slice percentages, and accumulated radians.
- Added prepared pie-slice cached state so a later rendering phase can draw
  directly from validated angle data instead of recalculating in `OnPaint()`.
- Verified the Phase 8 implementation by compiling the host `GoTwinAppEditor`
  target successfully on July 16, 2026.
- Documented the Phase 8 milestone verification for pie-slice angle
  calculation.
- Implemented the Phase 9 `SPieChart` rendering path using triangulated custom
  Slate vertices and indices.
- Rendered complete multi-slice pie geometry from prepared slice-angle state
  instead of recalculating proportions inside `OnPaint()`.
- Verified the Phase 9 implementation by compiling the host `GoTwinAppEditor`
  target successfully on July 16, 2026.
- Documented the Phase 9 milestone verification for pie-slice rendering.
