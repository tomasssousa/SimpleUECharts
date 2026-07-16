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
