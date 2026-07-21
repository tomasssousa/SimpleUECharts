#include "Data/ChartStyles.h"

#include "Styling/CoreStyle.h"

FChartStyle::FChartStyle()
    : BackgroundColor(FLinearColor::Transparent)
    , TextColor(FLinearColor::White)
    , Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 10))
    , Padding(8.0f)
    , LabelPadding(4.0f)
    , ChartScale(1.0f)
    , TextScale(1.0f)
{
}

FBarChartStyle::FBarChartStyle()
    : BarSpacing(8.0f)
    , MinimumBarWidth(8.0f)
    , MaximumBarWidth(128.0f)
    , bShowLabels(true)
    , bShowValues(true)
    , bShowYAxis(true)
    , bShowGridLines(false)
    , AxisColor(FLinearColor::White)
    , GridLineColor(0.25f, 0.25f, 0.25f, 1.0f)
    , bEnableHover(true)
    , HoverTint(FLinearColor::White)
    , HoverOpacityMultiplier(1.0f)
{
}

FPieChartStyle::FPieChartStyle()
    : StartAngle(0.0f)
    , SliceSpacing(0.0f)
    , InnerRadius(0.0f)
    , PieHorizontalAlignment(0.5f)
    , bShowLabels(true)
    , bShowValues(true)
    , bShowPercentages(true)
    , bShowLegend(true)
    , LegendSpacing(16.0f)
{
}
