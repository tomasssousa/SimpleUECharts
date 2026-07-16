# SimpleUECharts

Lightweight Unreal Engine charts for UMG and Slate.

## Repository Status

This repository now has the runtime plugin/module skeleton in place plus the
shared chart data contract used by both chart widget families.

## Current Progress

- Phase 1 milestone verified: the plugin is configured as a Runtime plugin.
- Phase 2 milestone verified in code: `FChartDataPoint` is exposed to C++ and
  Blueprint and both chart widgets accept `TArray<FChartDataPoint>`.

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

## Current Focus

The repository is organized for future implementation of:

- Bar charts
- Pie charts
- UMG and Slate integration
