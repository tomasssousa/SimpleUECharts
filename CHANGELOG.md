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
- Implemented Phase 10 presentation controls for the pie chart, including
  labels, values, percentages, legend, start angle, slice spacing, and inner
  radius.
- Synchronized the new phase 10 pie-chart presentation properties from
  `UPieChartWidget` into `SPieChart`.
- Added donut-style pie rendering and legend/text layout support on top of the
  existing slice geometry path.
- Verified the Phase 10 implementation by compiling the host `GoTwinAppEditor`
  target successfully on July 16, 2026.
- Documented the Phase 10 milestone verification for pie-chart presentation
  features.
- Adjusted the phase 10 pie-chart presentation toward a side-legend layout,
  keeping labels and totals beside the chart instead of overlaying slice text.
- Refactored `SBarChart` so normalized bar data and size-dependent layout are
  cached outside `OnPaint()`.
- Refactored `SPieChart` so cached slice-angle data now feeds a cached
  size-dependent mesh and legend layout instead of rebuilding the full chart in
  the render path.
- Verified the Phase 11 implementation by compiling the host `GoTwinAppEditor`
  target successfully on July 16, 2026.
- Documented the Phase 11 milestone verification for separating calculation
  from rendering.
- Fixed the phase 11 pie-chart cached mesh path so the current widget render
  transform is reapplied during `OnPaint()`, keeping the chart scaled and
  positioned inside its allotted area.
- Integrated the phase 12 bar-chart styling path so `UBarChartWidget` now
  exposes reusable `FChartStyle` and `FBarChartStyle` properties instead of
  relying on separate bar-chart presentation fields.
- Updated `SBarChart` to render from reusable style structs for background,
  text, padding, spacing, label/value visibility, axis/grid colors, and
  preferred minimum/maximum bar-width behavior.
- Verified the phase 12 bar-chart styling implementation by compiling the host
  `GoTwinAppEditor` target successfully on July 16, 2026.
- Documented that the bar-chart portion of phase 12 is complete while the
  overall roadmap milestone remains open until the pie chart adopts the same
  styling system.
