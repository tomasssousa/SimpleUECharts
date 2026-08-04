# SimpleUECharts

`SimpleUECharts` is a lightweight Unreal Engine runtime plugin for building
native chart widgets with UMG and Slate.

Its goal is to make common UI charts easy to add to an Unreal project without
introducing external charting libraries, editor-only systems, or per-frame
recalculation for ordinary updates. The plugin keeps the workflow simple:

```cpp
Chart->SetData(Data);
```

The current implementation focuses on reusable bar, pie, and time-series widgets
that can be updated from gameplay code, C++ UI code, or Blueprint.

## Target Language And Unreal Version

This plugin is primarily intended for Unreal Engine C++ projects.

- Language: C++
- Unreal UI stack: UMG + Slate
- Blueprint support: yes, through `BlueprintCallable` APIs and `BlueprintType`
  data/style structs
- Current implementation target: Unreal Engine 5.5

The module is configured as a Runtime plugin, so it is intended to work inside
regular game/application builds rather than only inside the editor.

## Features

The plugin currently provides:

- Native `UWidget` wrappers for bar, pie, and time-series charts
- Native Slate rendering through `SBarChart`, `SPieChart`, and `STimeSeriesChart`
- Generic multi-series timestamp/value data with automatic X/Y scaling
- Time-series axes, grid, markers, legend, hover tooltips, and series emphasis
- Shared chart data model through `FChartDataPoint`
- Shared styling model through `FChartStyle`, `FBarChartStyle`, and
  `FPieChartStyle`
- C++ and Blueprint data updates through `SetData`, `ClearData`, and
  `RefreshChart`
- Automatic bar-chart value scaling
- Shared chart scaling and text/legend scaling through `FChartStyle`
- Per-data-point colors
- Bar-chart labels, values, Y axis, and optional grid lines
- Pie-chart slice angle calculation and native Slate mesh rendering
- Pie-chart legend, value display, percentage display, start angle, slice
  spacing, donut-style inner radius, and configurable legend spacing
- Separation between chart calculation and chart rendering, with cached
  prepared state reused by `OnPaint()`

## Repository Structure

The repository is organized around a single runtime plugin module:

```text
SimpleUECharts/
|
+-- SimpleUECharts.uplugin
+-- README.md
+-- CHANGELOG.md
+-- docs/
|   +-- ROADMAP.md
+-- Source/
|   +-- SimpleUECharts/
|       +-- SimpleUECharts.Build.cs
|       +-- Public/
|       |   +-- Data/
|       |   |   +-- ChartDataPoint.h
|       |   |   +-- ChartStyles.h
|       |   |   +-- TimeSeriesData.h
|       |   +-- Widgets/
|       |   |   +-- BarChartWidget.h
|       |   |   +-- PieChartWidget.h
|       |   |   +-- TimeSeriesChartWidget.h
|       |   +-- Slate/
|       |       +-- SBarChart.h
|       |       +-- SPieChart.h
|       |       +-- STimeSeriesChart.h
|       +-- Private/
|           +-- SimpleUECharts.cpp
|           +-- Widgets/
|           |   +-- BarChartWidget.cpp
|           |   +-- PieChartWidget.cpp
|           |   +-- TimeSeriesChartWidget.cpp
|           +-- Slate/
|               +-- SBarChart.cpp
|               +-- SPieChart.cpp
|               +-- STimeSeriesChart.cpp
```

### What Each Part Does

- `SimpleUECharts.uplugin`
  Declares the plugin, its metadata, and the runtime module entry.

- `docs/ROADMAP.md`
  Defines the intended architecture, feature order, and long-term scope.

- `Source/SimpleUECharts/SimpleUECharts.Build.cs`
  Declares the module dependencies. The current runtime module depends on
  `Core`, `CoreUObject`, `Engine`, `Slate`, `SlateCore`, and `UMG`.

- `Public/Data/ChartDataPoint.h`
  Defines `FChartDataPoint`, the shared dataset item used by both chart types.

- `Public/Data/ChartStyles.h`
  Defines the reusable chart style structs shared by the UMG and Slate layers.

- `Public/Widgets/*`
  Declares the UMG-facing widget classes. These are the classes users interact
  with from C++ and Blueprint.

- `Private/Widgets/*`
  Implements the UMG widget behavior, including `RebuildWidget()`,
  property synchronization, and chart refresh APIs.

- `Public/Slate/*`
  Declares the lower-level Slate widgets responsible for layout and drawing.

- `Private/Slate/*`
  Implements the Slate rendering, chart calculations, cached layout, and
  `OnPaint()` logic.

## Installation

There are two common ways to add the plugin to a project.

### Option 1: Install As A Project Plugin

1. Copy the `SimpleUECharts` folder into your project's `Plugins/` directory.
2. Your project structure should look like this:

```text
MyProject/
|
+-- Plugins/
|   +-- SimpleUECharts/
|       +-- SimpleUECharts.uplugin
+-- Source/
+-- Content/
```

3. Open the Unreal project.
4. If prompted, rebuild the project modules.
5. In the Unreal Editor, open `Edit -> Plugins` and confirm that
   `SimpleUECharts` is enabled.

### Option 2: Install As An Engine Plugin

1. Copy the `SimpleUECharts` folder into your Unreal Engine installation's
   `Engine/Plugins/` directory.
2. Open your project.
3. Enable the plugin from `Edit -> Plugins` if needed.

For most teams, the project-plugin approach is easier to version and share.

## Adding The Plugin To A C++ Project

If your project uses C++, make sure the plugin is enabled in the `.uproject`
file or in the Plugins UI, then regenerate project files if Unreal asks for it.

You can then include the widget headers where needed:

```cpp
#include "Widgets/BarChartWidget.h"
#include "Widgets/PieChartWidget.h"
#include "Widgets/TimeSeriesChartWidget.h"
#include "Data/ChartDataPoint.h"
#include "Data/ChartStyles.h"
```

## How To Use The Plugin

The plugin is designed to be used in either UMG Designer, Blueprint, or C++.

### 1. Add A Widget In UMG

If the plugin is enabled, `Bar Chart` and `Pie Chart` should appear in the UMG
palette under `Simple UE Charts`.

You can drag either widget into a `Widget Blueprint` and then configure:

- `Data`
- `ChartStyle`
- `BarChartStyle` for bar charts
- `PieChartStyle` for pie charts

### 2. Prepare Chart Data

Both charts use the same shared data structure:

```cpp
TArray<FChartDataPoint> Data = {
    FChartDataPoint(FText::FromString(TEXT("Chrome")), 60.0f, FLinearColor::Blue),
    FChartDataPoint(FText::FromString(TEXT("Firefox")), 25.0f, FLinearColor::Orange),
    FChartDataPoint(FText::FromString(TEXT("Other")), 15.0f, FLinearColor::Gray)
};
```

### 3. Update A Chart From C++

Example with a bound bar chart:

```cpp
UPROPERTY(meta = (BindWidget))
UBarChartWidget* BarChart;

void UMyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BarChart)
    {
        TArray<FChartDataPoint> Data = {
            FChartDataPoint(FText::FromString(TEXT("Jan")), 20.0f, FLinearColor::Red),
            FChartDataPoint(FText::FromString(TEXT("Feb")), 50.0f, FLinearColor::Green),
            FChartDataPoint(FText::FromString(TEXT("Mar")), 100.0f, FLinearColor::Blue)
        };

        BarChart->SetData(Data);
    }
}
```

Example with a bound pie chart:

```cpp
UPROPERTY(meta = (BindWidget))
UPieChartWidget* PieChart;

void UMyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (PieChart)
    {
        TArray<FChartDataPoint> Data = {
            FChartDataPoint(FText::FromString(TEXT("A")), 50.0f, FLinearColor::Red),
            FChartDataPoint(FText::FromString(TEXT("B")), 30.0f, FLinearColor::Green),
            FChartDataPoint(FText::FromString(TEXT("C")), 20.0f, FLinearColor::Blue)
        };

        PieChart->SetData(Data);
    }
}
```

### 4. Style A Chart

Bar-chart styling example:

```cpp
BarChart->ChartStyle.BackgroundColor = FLinearColor(0.04f, 0.04f, 0.06f, 1.0f);
BarChart->ChartStyle.TextColor = FLinearColor::White;
BarChart->ChartStyle.Padding = FMargin(16.0f, 12.0f);
BarChart->ChartStyle.ChartScale = 0.9f;
BarChart->ChartStyle.TextScale = 1.15f;

BarChart->BarChartStyle.BarSpacing = 12.0f;
BarChart->BarChartStyle.bShowGridLines = true;
BarChart->BarChartStyle.AxisColor = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
BarChart->BarChartStyle.GridLineColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);

BarChart->RefreshChart();
```

Pie-chart styling example:

```cpp
PieChart->ChartStyle.BackgroundColor = FLinearColor(0.06f, 0.06f, 0.08f, 1.0f);
PieChart->ChartStyle.TextColor = FLinearColor::White;
PieChart->ChartStyle.Padding = FMargin(14.0f);
PieChart->ChartStyle.ChartScale = 0.85f;
PieChart->ChartStyle.TextScale = 1.2f;

PieChart->PieChartStyle.StartAngle = -90.0f;
PieChart->PieChartStyle.SliceSpacing = 6.0f;
PieChart->PieChartStyle.InnerRadius = 0.45f;
PieChart->PieChartStyle.LegendSpacing = 28.0f;
PieChart->PieChartStyle.bShowLegend = true;

PieChart->RefreshChart();
```

### 5. Update A Chart From Blueprint

The plugin also supports Blueprint workflows:

1. Add a `Bar Chart` or `Pie Chart` widget to your UMG widget.
2. Store a reference to it.
3. Build an array of `Chart Data Point` values.
4. Call `SetData`.
5. Optionally adjust `ChartStyle`, `BarChartStyle`, or `PieChartStyle`.
6. Call `RefreshChart` if you changed style values after creation.

## Scaling Controls

The shared `FChartStyle` struct exposes:

- `ChartScale`
  Scales bar and pie chart geometry.
- `TextScale`
  Scales bar labels, value text, Y-axis labels, and pie-chart legend text and
  swatches.

The pie-chart-specific `FPieChartStyle` also exposes:

- `LegendSpacing`
  Controls the gap between the pie graphic and the legend block.

These settings are available in both C++ and Blueprint through the widget
style properties.

## Current Scope

The current chart types are:

- `UBarChartWidget` / `SBarChart`
- `UPieChartWidget` / `SPieChart`
- `UTimeSeriesChartWidget` / `STimeSeriesChart`

The plugin deliberately stays focused on a small Unreal-native feature set.

Out of scope for the current version:

- External charting libraries
- Python rendering paths
- Additional chart families beyond bar, pie, and time series
- Complex editor tooling
- Real-time per-frame chart recomputation by default

## Notes

- The plugin uses native Slate drawing, including custom Slate vertices for pie
  slices.
- The intended update model is event-driven rather than tick-driven.
- Public style defaults are implemented in module source rather than inline in
  the public header, which helps keep exported header dependencies narrower.
- The project roadmap and planned milestones are documented in
  `docs/ROADMAP.md`.
