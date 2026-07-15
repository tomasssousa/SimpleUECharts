# SimpleUECharts — Development Roadmap

**SimpleUECharts** is an open-source Unreal Engine plugin for creating lightweight, native chart widgets directly inside **UMG/Slate**.

The initial scope is intentionally small:

- Bar Chart
- Pie Chart
- C++ API
- Blueprint API
- Native Unreal Engine rendering
- No Python
- No external charting libraries
- No per-frame data recalculation unless explicitly requested

The main goal is to provide simple chart widgets that can be added to a UMG interface and updated whenever the underlying data changes.

---

# 1. Project Goals

SimpleUECharts should provide:

- Native Unreal Engine chart rendering.
- Easy integration with UMG.
- A clean C++ API.
- Blueprint-accessible functions and properties.
- Event-driven chart updates.
- Reusable chart data structures.
- Lightweight runtime dependencies.
- A simple codebase that is easy to understand and contribute to.

The first stable version should support:

- Vertical bar charts.
- Pie charts.
- Per-data-point colours.
- Labels.
- Values and percentages.
- Configurable spacing and styling.
- Runtime data updates.

---

# 2. High-Level Architecture

The plugin should separate the Unreal UObject/UMG layer from the lower-level Slate rendering layer.

```text
UMG
│
├── UBarChartWidget
│       ↓
│    SBarChart
│
└── UPieChartWidget
        ↓
     SPieChart
```

The responsibilities are divided as follows:

## UMG / UObject Layer

Responsible for:

- Being visible in the UMG Widget Palette.
- Exposing properties to the Unreal Editor.
- Exposing functions to Blueprint.
- Receiving data from gameplay/application code.
- Synchronising properties with the Slate widget.

Main classes:

```text
UBarChartWidget
UPieChartWidget
```

## Slate Layer

Responsible for:

- Chart layout.
- Geometry calculation.
- Native rendering.
- Drawing bars.
- Drawing pie slices.
- Drawing axes, labels and other visual elements.

Main classes:

```text
SBarChart
SPieChart
```

---

# 3. Suggested Plugin Structure

```text
SimpleUECharts/
│
├── SimpleUECharts.uplugin
│
├── README.md
├── ROADMAP.md
├── LICENSE
│
└── Source/
    └── SimpleUECharts/
        │
        ├── SimpleUECharts.Build.cs
        │
        ├── Public/
        │   │
        │   ├── Data/
        │   │   ├── ChartDataPoint.h
        │   │   └── ChartStyles.h
        │   │
        │   ├── Widgets/
        │   │   ├── BarChartWidget.h
        │   │   └── PieChartWidget.h
        │   │
        │   └── Slate/
        │       ├── SBarChart.h
        │       └── SPieChart.h
        │
        └── Private/
            │
            ├── SimpleUECharts.cpp
            │
            ├── Widgets/
            │   ├── BarChartWidget.cpp
            │   └── PieChartWidget.cpp
            │
            └── Slate/
                ├── SBarChart.cpp
                └── SPieChart.cpp
```

Additional folders may be added later for:

```text
Tests/
Examples/
Editor/
Documentation/
```

---

# 4. Plugin Dependencies

The runtime module will likely require:

```csharp
PublicDependencyModuleNames.AddRange(
    new string[]
    {
        "Core",
        "CoreUObject",
        "Engine",
        "UMG",
        "Slate",
        "SlateCore"
    }
);
```

The first version should remain a **Runtime plugin** so that charts work in packaged applications.

---

# 5. Shared Data Model

Both charts should use the same basic data structure.

```cpp
USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FChartDataPoint
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FText Label;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float Value = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor Color = FLinearColor::White;
};
```

Example:

```cpp
TArray<FChartDataPoint> Data;
```

```text
Chrome   → 60
Firefox  → 25
Other    → 15
```

The same dataset can be rendered as:

```text
Bar Chart
```

or:

```text
Pie Chart
```

This avoids creating separate and incompatible APIs for each chart type.

---

# 6. Update Model

Charts should be **event-driven**.

The chart should be recalculated only when:

- The widget is created.
- The page is opened.
- New data is provided.
- A relevant visual property changes.
- The application explicitly requests a refresh.

Example:

```cpp
Chart->SetData(NewData);
```

Internal flow:

```text
SetData()
   ↓
Store Data
   ↓
Validate Data
   ↓
Recalculate Chart Geometry
   ↓
Invalidate Widget
   ↓
Slate Repaints
```

Avoid putting expensive calculations inside:

```text
Tick()
OnPaint()
```

The rendering function should ideally use data and geometry that have already been prepared.

---

# 7. Core API

The initial public API should remain simple.

## Common Operations

```cpp
SetData(...)
ClearData()
AddDataPoint(...)
RemoveDataPoint(...)
RefreshChart()
```

Possible example:

```cpp
BarChart->SetData(Data);
```

```cpp
PieChart->SetData(Data);
```

The API should also be exposed to Blueprint where appropriate.

---

# 8. Phase 1 — Create the Runtime Plugin

## Goals

- Create the `SimpleUECharts` plugin.
- Configure it as a Runtime plugin.
- Add the required Unreal Engine modules.
- Confirm that the plugin compiles.
- Confirm that it can be enabled and disabled.
- Confirm that it works in a clean Unreal Engine project.

## Milestone

```text
Plugin loads successfully.
```

No chart rendering is required yet.

---

# 9. Phase 2 — Implement the Shared Data Model

Create:

```text
FChartDataPoint
```

The first version should contain:

```text
Label
Value
Color
```

Optional fields can be added later.

Possible future extensions:

```text
ID
Tooltip text
Metadata
Visibility
Custom formatting
```

## Milestone

The plugin can receive a reusable array of chart data points.

---

# 10. Phase 3 — Create the Bar Chart UMG Widget

Start with the bar chart because its rendering logic is simpler.

Create:

```text
UBarChartWidget
```

Responsibilities:

- Expose chart properties to the Editor.
- Expose `SetData()` to C++ and Blueprint.
- Create the underlying `SBarChart`.
- Synchronise properties with Slate.

Suggested class structure:

```cpp
UCLASS(meta = (DisplayName = "Bar Chart"))
class SIMPLEUECHARTS_API UBarChartWidget : public UWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    TArray<FChartDataPoint> Data;

    UFUNCTION(BlueprintCallable, Category = "Chart")
    void SetData(const TArray<FChartDataPoint>& NewData);

protected:

    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void SynchronizeProperties() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:

    TSharedPtr<class SBarChart> MyBarChart;
};
```

Basic widget creation:

```cpp
TSharedRef<SWidget> UBarChartWidget::RebuildWidget()
{
    MyBarChart = SNew(SBarChart);

    return MyBarChart.ToSharedRef();
}
```

## Milestone

`Bar Chart` appears in the UMG Widget Palette.

---

# 11. Phase 4 — Create the SBarChart Slate Widget

Create:

```text
SBarChart
```

Suggested base:

```cpp
class SBarChart : public SLeafWidget
{
public:

    SLATE_BEGIN_ARGS(SBarChart) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetData(const TArray<FChartDataPoint>& NewData);

    virtual int32 OnPaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled
    ) const override;

private:

    TArray<FChartDataPoint> Data;
};
```

The first rendering test should be extremely small:

```text
Draw one rectangle.
```

Then:

```text
Draw N rectangles.
```

## Milestone

The Slate widget can render one or more bars.

---

# 12. Phase 5 — Implement Bar Chart Scaling

Given:

```text
Values = 20, 50, 100
```

Find the maximum value:

```cpp
MaxValue = 100.0f;
```

Normalise each value:

```cpp
NormalizedValue = Value / MaxValue;
```

Result:

```text
20  → 0.20
50  → 0.50
100 → 1.00
```

Bar height:

```cpp
BarHeight = NormalizedValue * ChartHeight;
```

Bar Y position:

```cpp
BarY = ChartHeight - BarHeight;
```

Remember that in UI coordinates:

```text
(0, 0) ───────────────────► X
  │
  │
  │
  ▼
  Y
```

The origin is at the top-left.

## Initial bar chart behaviour

Support:

- One bar per data point.
- Automatic scaling.
- Individual colours.
- Configurable spacing.
- Configurable chart padding.

Do not implement advanced features yet.

Avoid initially adding:

- Stacked bars.
- Multiple datasets.
- Negative values.
- Horizontal bars.
- Animations.
- Tooltips.

## Milestone

A valid dataset produces a correctly scaled vertical bar chart.

---

# 13. Phase 6 — Add Bar Chart Labels and Axes

After the basic bars work, add optional presentation features.

Potential properties:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
bool bShowLabels = true;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
bool bShowValues = true;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
bool bShowYAxis = true;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
bool bShowGridLines = false;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
float BarSpacing = 10.0f;
```

Target result:

```text
100 ┤                 ███
    │                 ███
 75 ┤          ███    ███
    │          ███    ███
 50 ┤          ███    ███
    │   ███    ███    ███
 25 ┤   ███    ███    ███
    └───────────────────────
        Jan    Feb    Mar
```

## Milestone

The bar chart is usable in a real application UI.

---

# 14. Phase 7 — Create the Pie Chart UMG Widget

After the complete data flow has been validated using the bar chart, create:

```text
UPieChartWidget
```

The architecture mirrors the bar chart:

```text
UPieChartWidget
       ↓
   SPieChart
       ↓
    OnPaint()
```

The UMG wrapper should expose:

```text
Data
SetData()
ClearData()
RefreshChart()
```

## Milestone

`Pie Chart` appears in the UMG Widget Palette and can receive data.

---

# 15. Phase 8 — Implement Pie Chart Mathematics

Given:

```text
A = 50
B = 30
C = 20
```

Calculate the total:

```cpp
Total = 100.0f;
```

Calculate each percentage:

```cpp
Percentage = Value / Total;
```

Convert the percentage into an angle:

```cpp
SliceAngle = Percentage * 2.0f * PI;
```

Equivalent result:

```text
A = 50% → 180°
B = 30% → 108°
C = 20% → 72°
```

Basic algorithm:

```cpp
float CurrentAngle = 0.0f;

for (const FChartDataPoint& Point : Data)
{
    const float Percentage = Point.Value / Total;
    const float SliceAngle = Percentage * 2.0f * PI;

    DrawSlice(
        CurrentAngle,
        CurrentAngle + SliceAngle
    );

    CurrentAngle += SliceAngle;
}
```

## Milestone

The plugin correctly calculates all pie slice angles.

---

# 16. Phase 9 — Render Pie Slices

A pie slice can be approximated using triangles.

For points along the circumference:

```cpp
X = CenterX + FMath::Cos(Angle) * Radius;
Y = CenterY + FMath::Sin(Angle) * Radius;
```

Each segment can be represented as:

```text
Center
PointAt(CurrentAngle)
PointAt(NextAngle)
```

Conceptually:

```text
             P1
            /  \
           /    \
     Center ---- P2
```

A larger number of segments produces a smoother circular edge.

Pseudo-algorithm:

```cpp
for (
    float Angle = StartAngle;
    Angle < EndAngle;
    Angle += Step
)
{
    AddTriangle(
        Center,
        PointAt(Angle),
        PointAt(Angle + Step)
    );
}
```

The implementation should build the required vertices and indices for each slice and submit the geometry through Slate rendering.

Important considerations:

- Avoid visible gaps caused by floating-point precision.
- Clamp the final segment to the exact slice end angle.
- Handle zero and negative values safely.
- Avoid division by zero when the total is zero.
- Keep geometry resolution configurable or internally sensible.

## Milestone

The plugin renders a complete multi-slice pie chart.

---

# 17. Phase 10 — Add Pie Chart Presentation Features

Add optional properties such as:

```text
Show Labels
Show Values
Show Percentages
Show Legend
Start Angle
Slice Gap
Inner Radius
```

Suggested properties:

```cpp
float StartAngle;
float SliceSpacing;
bool bShowLabels;
bool bShowValues;
bool bShowPercentages;
bool bShowLegend;
float InnerRadius;
```

Supporting `InnerRadius` creates a natural path toward donut charts.

```text
InnerRadius = 0
    ↓
Pie Chart

InnerRadius > 0
    ↓
Donut Chart
```

## Milestone

The pie chart is ready for real application use.

---

# 18. Phase 11 — Separate Calculation from Rendering

Both chart types should follow the same internal pattern.

```text
SetData()
   │
   ▼
ValidateData()
   │
   ▼
RecalculateChart()
   │
   ├── normalisation
   ├── positions
   ├── angles
   └── cached geometry
           │
           ▼
        OnPaint()
           │
           └── draw prepared data
```

Avoid:

```cpp
OnPaint()
{
    // Recalculate the entire chart every time.
}
```

Prefer:

```cpp
SetData(...)
{
    Data = NewData;

    RecalculateChart();

    Invalidate(...);
}
```

This is especially appropriate for SimpleUECharts because the intended use case does not require updating chart values every frame.

---

# 19. Phase 12 — Styling

Once both charts are functional, move visual configuration into reusable style structures.

## Common Chart Style

Possible structure:

```text
FChartStyle
```

Properties:

```text
Background Color
Text Color
Font
Padding
Label Padding
```

## Bar Chart Style

Possible structure:

```text
FBarChartStyle
```

Properties:

```text
Bar Spacing
Minimum Bar Width
Maximum Bar Width
Show Labels
Show Values
Show Y Axis
Show Grid Lines
Axis Color
Grid Line Color
```

## Pie Chart Style

Possible structure:

```text
FPieChartStyle
```

Properties:

```text
Start Angle
Slice Spacing
Inner Radius
Show Labels
Show Values
Show Percentages
Show Legend
```

## Milestone

Charts can be styled without changing implementation code.

---

# 20. Version Roadmap

## v0.1 — Minimal Bar Chart

Goals:

- Runtime plugin.
- Shared `FChartDataPoint`.
- UMG Bar Chart widget.
- Slate Bar Chart widget.
- C++ API.
- Blueprint API.
- Multiple bars.
- Automatic scaling.
- Per-bar colours.
- Configurable spacing.

Target API:

```cpp
BarChart->SetData(Data);
```

Target result:

```text
███

███    ███

███    ███    ███
```

---

## v0.2 — Complete Bar Chart

Add:

- Labels.
- Values.
- Chart padding.
- Background.
- Y axis.
- Grid lines.
- Better sizing behaviour.
- Empty-state handling.
- Validation.

---

## v0.3 — Minimal Pie Chart

Add:

- UMG Pie Chart widget.
- Slate Pie Chart widget.
- Total calculation.
- Percentage calculation.
- Slice angle calculation.
- Pie slice rendering.
- Per-slice colours.

Target API:

```cpp
PieChart->SetData(Data);
```

---

## v0.4 — Complete Pie Chart

Add:

- Labels.
- Values.
- Percentages.
- Legend.
- Start angle.
- Slice spacing.
- Configurable geometry resolution.

---

## v0.5 — Shared Styling System

Add:

- Common chart styling.
- Bar chart styling.
- Pie chart styling.
- Better Editor-exposed properties.
- Consistent defaults.

---

## v0.6 — Interaction and Quality Improvements

Potential additions:

- Hover detection.
- Click events.
- Tooltips.
- Selected data point state.
- Highlighting.
- Better text placement.
- Responsive labels.
- Accessibility considerations.

These features are optional and should not delay the first stable release.

---

## v0.7 — Testing and Example Project

Add:

- Example UMG screens.
- Example C++ usage.
- Example Blueprint usage.
- Edge-case tests.
- Packaged build tests.
- Documentation screenshots.

Test datasets should include:

```text
Empty dataset
Single data point
Many data points
All zero values
Very large values
Very small values
Repeated values
Long labels
Zero-value pie slices
Invalid negative pie values
```

---

## v1.0 — Stable Public Release

Requirements:

- Stable C++ API.
- Stable Blueprint API.
- Bar Chart complete.
- Pie Chart complete.
- Documentation.
- Examples.
- Packaged build verified.
- No known critical rendering issues.
- Open-source licence included.
- Contribution guidelines.
- Changelog.
- Versioning policy.

---

# 21. Recommended Development Order

Follow this order:

```text
1. Create Runtime Plugin
        ↓
2. Create FChartDataPoint
        ↓
3. Create UBarChartWidget
        ↓
4. Create SBarChart
        ↓
5. Draw one rectangle
        ↓
6. Draw N bars
        ↓
7. Implement automatic scaling
        ↓
8. Add colours and spacing
        ↓
9. Test SetData()
        ↓
10. Add labels and optional axes
        ↓
11. Create UPieChartWidget
        ↓
12. Create SPieChart
        ↓
13. Draw one triangle
        ↓
14. Draw one circular sector
        ↓
15. Divide the circle using chart data
        ↓
16. Add labels, values and legend
        ↓
17. Add shared styling
        ↓
18. Test packaged builds
        ↓
19. Create examples and documentation
        ↓
20. Release v1.0
```

---

# 22. First Major Milestone

The first important milestone should be:

> A developer can drag a **Bar Chart** widget from the UMG Palette, call `SetData()`, and see the chart update correctly.

Example:

```cpp
UPROPERTY(meta = (BindWidget))
UBarChartWidget* BarChart;
```

```cpp
BarChart->SetData(Data);
```

Once this works, the most important parts of the plugin architecture are already validated:

- Plugin module.
- UObject/UMG layer.
- Slate layer.
- Data flow.
- Runtime updates.
- Rendering.
- Blueprint exposure.

The pie chart then becomes primarily a geometry and rendering problem.

---

# 23. Suggested API Principles

The public API should remain:

- Small.
- Predictable.
- Consistent between chart types.
- Usable from both C++ and Blueprint.

Prefer:

```cpp
Chart->SetData(Data);
```

over APIs requiring many individual setup calls.

Prefer shared concepts:

```text
Data
Style
Refresh
Clear
```

Avoid exposing internal Slate implementation details to users of the plugin.

---

# 24. Scope Control

The first stable version should focus on doing two chart types well.

Initial scope:

```text
Bar Chart
Pie Chart
```

Out of scope for the first release:

```text
Line Chart
Scatter Plot
Radar Chart
Heatmap
Stacked Bar Chart
3D Charts
Real-time high-frequency telemetry
Complex animation systems
```

These can be considered later if the architecture remains suitable.

---

# 25. Possible Future Features

After v1.0, possible extensions include:

- Donut Chart.
- Horizontal Bar Chart.
- Grouped Bar Chart.
- Stacked Bar Chart.
- Animated transitions.
- Tooltips.
- Clickable chart elements.
- Custom legends.
- Custom label formatters.
- Percentage/value formatter callbacks.
- Chart themes.
- Data sorting.
- Automatic colour palettes.
- Editor preview data.
- Additional chart types.

The plugin should not be designed around these features before they are actually needed.

---

# 26. Open-Source Repository Checklist

Recommended repository files:

```text
README.md
ROADMAP.md
LICENSE
CONTRIBUTING.md
CHANGELOG.md
CODE_OF_CONDUCT.md
```

The repository should clearly document:

- Supported Unreal Engine versions.
- Installation instructions.
- C++ usage.
- Blueprint usage.
- Known limitations.
- Contribution workflow.
- Licence.

For an open-source plugin, keep the build and installation process as simple as possible.

---

# 27. Definition of Done for v1.0

SimpleUECharts v1.0 is complete when:

- [ ] The plugin compiles successfully.
- [ ] The plugin works as a Runtime plugin.
- [ ] Bar Chart is available in UMG.
- [ ] Pie Chart is available in UMG.
- [ ] Both charts support C++.
- [ ] Both charts support Blueprint.
- [ ] Both charts accept `TArray<FChartDataPoint>`.
- [ ] Bar Chart scales values automatically.
- [ ] Pie Chart calculates slice proportions correctly.
- [ ] Both charts support per-item colours.
- [ ] Both charts update when `SetData()` is called.
- [ ] Empty and invalid datasets are handled safely.
- [ ] Packaged builds are tested.
- [ ] Example usage is documented.
- [ ] The repository includes an open-source licence.
- [ ] The public API is documented.
- [ ] A first tagged release is published.

---

# 28. Final Design Principle

The core design philosophy of SimpleUECharts should be:

```text
Simple data in
      ↓
Native Unreal chart out
```

The plugin should remain:

```text
Simple
Native
Reusable
Open source
UMG-friendly
C++-friendly
Blueprint-friendly
```

The initial objective is not to compete with large data visualisation frameworks.

The objective is to make basic charts in Unreal Engine easy.

```cpp
Chart->SetData(Data);
```

That should remain the central experience of the plugin.
