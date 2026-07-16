# SimpleUECharts

Lightweight Unreal Engine charts for UMG and Slate.

## Repository Status

This repository now has the runtime plugin/module skeleton in place plus the
shared chart data contract used by both chart widget families.

## Current Progress

- Phase 1 milestone verified: the plugin is configured as a Runtime plugin.
- Phase 2 milestone verified in code: `FChartDataPoint` is exposed to C++ and
  Blueprint and both chart widgets accept `TArray<FChartDataPoint>`.
- Phase 3 milestone verified in code: `UBarChartWidget` is a concrete `UWidget`
  with editor-visible display metadata, Blueprint API, Slate synchronization,
  and an explicit UMG palette category.
- Phase 4 milestone verified in code: `SBarChart` now renders one rectangle per
  data point in `OnPaint()`, establishing the first visible native Slate output
  for bar charts.
- Phase 5 milestone verified in code: bar heights now scale from
  `FChartDataPoint::Value`, while `BarSpacing` and `ChartPadding` are
  configurable from the UMG wrapper.
- Phase 6 milestone verified in code: labels, values, Y axis, and optional grid
  lines are now available for the bar chart presentation layer.
- Phase 7 milestone verified in code: `UPieChartWidget` is exposed as a UMG
  widget with shared chart data input, Blueprint API, Slate synchronization,
  and an explicit UMG palette category.
- Phase 8 milestone verified in code and host-project build on July 16, 2026:
  `SPieChart` now prepares validated pie-slice percentages and accumulated
  angles from shared chart data, ready for rendering in the next phase.
- Phase 9 milestone verified in code and host-project build on July 16, 2026:
  `SPieChart` now triangulates and renders complete multi-slice pie geometry in
  native Slate from the prepared slice-angle state.
- Phase 10 milestone verified in code and host-project build on July 16, 2026:
  the pie chart now supports presentation controls for labels, values,
  percentages, legend, start angle, slice spacing, and donut-style inner
  radius.
  The current presentation emphasis is a side legend layout rather than
  overlaying text directly on the slices.
- Phase 11 milestone verified in code and host-project build on July 16, 2026:
  both chart types now separate data preparation from rendering, with cached
  chart state and cached size-dependent layout reused by `OnPaint()`.
  A follow-up fix on July 16, 2026 reapplied the current widget render
  transform to cached pie-chart mesh data so the pie chart scales and fits the
  allotted widget area correctly.
- Phase 12 bar-chart styling implementation verified in code and host-project
  build on July 16, 2026: `UBarChartWidget` now exposes reusable `FChartStyle`
  and `FBarChartStyle` structs, and `SBarChart` renders from those style
  objects for background, text, padding, spacing, label/value visibility,
  axis/grid colors, and preferred bar-width limits.
  The overall phase 12 milestone remains open because the pie chart has not yet
  been migrated to the same styling system.

## Key Files

- `docs/ROADMAP.md` contains the full development plan.
- `SimpleUECharts.uplugin` defines the Unreal Engine plugin.
- `Source/SimpleUECharts/` contains the runtime module skeleton.
- `Source/SimpleUECharts/Public/Data/ChartDataPoint.h` defines the shared data
  model.
- `Source/SimpleUECharts/Public/Data/ChartStyles.h` defines the reusable chart
  style structs.

## Shared Data Model

`FChartDataPoint` is the reusable dataset item shared by bar and pie charts.

```cpp
TArray<FChartDataPoint> Data = {
    FChartDataPoint(FText::FromString(TEXT("Chrome")), 60.0f, FLinearColor::Blue),
    FChartDataPoint(FText::FromString(TEXT("Firefox")), 25.0f, FLinearColor::Orange),
    FChartDataPoint(FText::FromString(TEXT("Other")), 15.0f, FLinearColor::Gray)
};
```

The same array can be passed to either widget API:

```cpp
BarChart->SetData(Data);
PieChart->SetData(Data);
```

## Bar Chart Styling

Phase 12 has now started on the bar-chart side through reusable style structs:

```cpp
BarChart->ChartStyle.BackgroundColor = FLinearColor(0.04f, 0.04f, 0.06f, 1.0f);
BarChart->ChartStyle.TextColor = FLinearColor::White;
BarChart->ChartStyle.Padding = FMargin(16.0f, 12.0f);

BarChart->BarChartStyle.BarSpacing = 12.0f;
BarChart->BarChartStyle.bShowGridLines = true;
BarChart->BarChartStyle.AxisColor = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
BarChart->BarChartStyle.GridLineColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);

BarChart->RefreshChart();
```

The bar chart styling path now covers:

- Background color.
- Text color and font.
- Chart padding and label padding.
- Bar spacing.
- Preferred minimum and maximum bar widths.
- Label, value, Y-axis, and grid-line visibility.
- Axis and grid-line colors.

## Phase 2 Milestone Check

The milestone "The plugin can receive a reusable array of chart data points"
is satisfied by the current codebase because:

- `FChartDataPoint` is a `USTRUCT(BlueprintType)`.
- `UBarChartWidget` stores and accepts `TArray<FChartDataPoint>`.
- `UPieChartWidget` stores and accepts `TArray<FChartDataPoint>`.
- `SBarChart` and `SPieChart` receive the same shared array type from the UMG
  layer.

## Phase 3 Milestone Check

The milestone "`Bar Chart` appears in the UMG Widget Palette" is satisfied in
the codebase because:

- `UBarChartWidget` is a non-abstract `UWidget` with `DisplayName = "Bar Chart"`.
- The widget exposes `Data`, `SetData`, `ClearData`, and `RefreshChart`.
- `RebuildWidget()` creates the underlying `SBarChart`.
- `SynchronizeProperties()` forwards the current `Data` array to Slate.
- `GetPaletteCategory()` places the widget under `Simple UE Charts` in the UMG
  palette when viewed in the editor.

## Phase 4 Milestone Check

The milestone "The Slate widget can render one or more bars" is satisfied in
the codebase because:

- `SBarChart::OnPaint()` now exits early only for empty data or invalid space.
- A valid `Data` array produces one `MakeBox()` draw call per item.
- Each bar uses the corresponding `FChartDataPoint::Color`.
- The implementation stays intentionally minimal and does not yet claim the
  automatic scaling work planned for Phase 5.

## Phase 5 Milestone Check

The milestone "A valid dataset produces a correctly scaled vertical bar chart"
is satisfied in the codebase because:

- `SBarChart::OnPaint()` now finds the maximum `Value` across the dataset.
- Each bar height is normalized against that maximum before drawing.
- Bar Y positions are derived from the normalized height, so bars grow upward
  from the bottom of the chart area.
- `FChartDataPoint::Color` still drives per-bar color.
- `UBarChartWidget` exposes `BarSpacing` and `ChartPadding`, and synchronizes
  both into `SBarChart`.

## Phase 6 Milestone Check

The milestone "The bar chart is usable in a real application UI" is satisfied
in the codebase because:

- `UBarChartWidget` now exposes `bShowLabels`, `bShowValues`, `bShowYAxis`, and
  `bShowGridLines`.
- `SBarChart` can draw Y-axis lines and tick labels.
- `SBarChart` can draw optional horizontal grid lines across the plot area.
- `SBarChart` can draw per-bar labels and per-bar values in addition to the
  scaled bar geometry.

## Phase 7 Milestone Check

The milestone "`Pie Chart` appears in the UMG Widget Palette and can receive
data" is satisfied in the codebase because:

- `UPieChartWidget` is a non-abstract `UWidget` with `DisplayName = "Pie Chart"`.
- The widget exposes `Data`, `SetData`, `ClearData`, and `RefreshChart`.
- `RebuildWidget()` creates the underlying `SPieChart`.
- `SynchronizeProperties()` forwards the current shared data array to Slate.
- `GetPaletteCategory()` places the widget under `Simple UE Charts` in the UMG
  palette when viewed in the editor.

## Phase 8 Milestone Check

The milestone "The plugin correctly calculates all pie slice angles" is
satisfied in the codebase because:

- `SPieChart::RecalculateChart()` sums only positive values into `TotalValue`.
- Each valid slice stores its `Percentage`, `StartAngleRadians`,
  `EndAngleRadians`, and `SweepAngleRadians` in prepared state.
- Non-positive values are excluded from slice generation, preventing invalid
  proportions and divide-by-zero paths for the phase 8 scope.
- The final slice is clamped to end exactly at `2 * PI`, reducing accumulated
  floating-point drift before rendering.
- The current implementation keeps calculation separate from `OnPaint()`, so
  phase 9 can render directly from prepared pie-slice geometry inputs.

## Phase 9 Milestone Check

The milestone "The plugin renders a complete multi-slice pie chart" is
satisfied in the codebase because:

- `SPieChart::OnPaint()` now exits only for empty slice state or invalid draw
  space, instead of being a placeholder.
- Each prepared slice is triangulated into custom Slate vertices and indices.
- Slice geometry is submitted through `FSlateDrawElement::MakeCustomVerts()`,
  which is the native Slate path for non-rectangular mesh rendering.
- The final segment of each slice is clamped to the exact stored end angle,
  helping the full circle close cleanly without visible drift between slices.
- A host-project `GoTwinAppEditor` build succeeded on July 16, 2026 with the
  pie-slice rendering implementation compiled into the plugin module.

## Phase 10 Milestone Check

The milestone "The pie chart is ready for real application use" is satisfied
in the codebase because:

- `UPieChartWidget` now exposes `bShowLabels`, `bShowValues`,
  `bShowPercentages`, `bShowLegend`, `StartAngle`, `SliceSpacing`, and
  `InnerRadius`, then synchronizes them into `SPieChart`.
- `SPieChart` can rotate the chart with `StartAngle` before slice layout is
  cached, instead of baking a fixed zero-angle start into the widget.
- `SPieChart` applies `SliceSpacing` during rendering to introduce visible gaps
  between adjacent slices.
- `SPieChart` supports `InnerRadius > 0`, which turns the chart into a donut
  layout while preserving the same shared data model.
- `SPieChart` can render a side legend alongside the pie geometry, showing the
  slice color, label, and optional value/percentage text without changing the
  dataset contract.
- A host-project `GoTwinAppEditor` build succeeded on July 16, 2026 with the
  full phase 10 pie-chart presentation feature set compiled into the plugin
  module.

## Phase 11 Milestone Check

The milestone "Separate calculation from rendering" is satisfied in the
codebase because:

- `SBarChart` now prepares normalized bar data outside `OnPaint()`, then caches
  size-dependent bar/tick layout separately before drawing from that prepared
  state.
- `SPieChart` continues to cache slice-angle data outside `OnPaint()`, and now
  also caches size-dependent mesh and legend layout before rendering.
- Property setters such as spacing, padding, legend toggles, and start angle
  now invalidate cached layout or chart state instead of relying on `OnPaint()`
  to recompute everything from scratch.
- `OnPaint()` in both chart widgets is now primarily responsible for drawing
  cached geometry/text rather than re-deriving the full chart model every
  repaint.
- A host-project `GoTwinAppEditor` build succeeded on July 16, 2026 with the
  phase 11 chart-cache refactor compiled into the plugin module.

## Phase 12 Milestone Check

The roadmap milestone "Charts can be styled without changing implementation
code" is not fully satisfied yet.

The current bar-chart portion is satisfied in the codebase because:

- `FChartStyle` and `FBarChartStyle` are now exposed on `UBarChartWidget`.
- `UBarChartWidget` synchronizes those reusable style structs directly into
  `SBarChart`.
- `SBarChart` now reads background, text, padding, spacing, visibility, axis
  color, grid-line color, and preferred bar-width limits from the style
  structs instead of hard-coded rendering defaults.
- A host-project `GoTwinAppEditor` build succeeded on July 16, 2026 with the
  bar-chart style integration compiled into the plugin module.

The overall phase 12 milestone remains open because:

- `UPieChartWidget` does not yet expose `FChartStyle` and `FPieChartStyle`.
- `SPieChart` still uses per-property presentation settings rather than the
  reusable styling contract introduced for the bar chart.

## Current Focus

The repository is organized for future implementation of:

- Bar charts
- Pie charts
- UMG and Slate integration
- Completing the shared styling system for pie charts
