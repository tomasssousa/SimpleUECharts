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
- Cleaned up includes by moving public style default constructors out of the
  exported header and removing a redundant include from the pie-chart render
  implementation.
- Updated the documentation set, including the roadmap-backed README rewrite.

## [v0.0.1]

## Notes

- The first release came out on July 16, 2026.
