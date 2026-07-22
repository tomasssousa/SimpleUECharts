# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

- Created the initial Runtime plugin structure and module setup for
  `SimpleUECharts`.
- Added the shared chart data model through `FChartDataPoint`.
- Implemented the bar chart widget stack:
  `UBarChartWidget` + `SBarChart`.
- Added bar-chart scaling, labels, values, Y axis, grid-line support, and
  reusable bar-chart styling through `FChartStyle` and `FBarChartStyle`.
- Implemented the pie chart widget stack:
  `UPieChartWidget` + `SPieChart`.
- Added pie-slice calculation, native Slate slice rendering, legend support,
  percentage/value presentation, start angle, slice spacing, donut-style inner
  radius, and reusable pie-chart styling through `FChartStyle` and
  `FPieChartStyle`.
- Refactored both chart types to separate calculation from rendering and reuse
  cached chart/layout state during painting.
- Added the shared Slate-to-UMG hover infrastructure for chart widgets,
  including pointer movement handling, hovered-index state, and hover cleanup.
- Implemented Bar Chart hover detection, Blueprint hover events, hover data
  exposure, and a non-layout-changing visual highlight for hovered bars.
- Implemented Pie Chart hover detection, Blueprint hover events, hover data
  exposure, donut-aware hit testing, and hover highlighting for hovered slices.
- Cleaned up includes by moving public style default constructors out of the
  exported header and removing a redundant include from the pie-chart render
  implementation.
- Updated the documentation set, including the roadmap-backed README rewrite.

## [v0.0.1]

- Added shared chart and text scaling controls for both chart types, plus
  configurable pie-chart legend spacing and responsive legend sizing.

## Notes

- The first release came out on July 16, 2026.
