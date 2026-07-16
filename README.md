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

## Key Files

- `docs/ROADMAP.md` contains the full development plan.
- `SimpleUECharts.uplugin` defines the Unreal Engine plugin.
- `Source/SimpleUECharts/` contains the runtime module skeleton.
- `Source/SimpleUECharts/Public/Data/ChartDataPoint.h` defines the shared data
  model.

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

## Current Focus

The repository is organized for future implementation of:

- Bar charts
- Pie charts
- UMG and Slate integration
