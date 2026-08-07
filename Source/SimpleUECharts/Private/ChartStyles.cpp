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
    , LegendSide(EPieChartLegendSide::Right)
    , LegendSpacing(16.0f)
    , UnselectedOpacity(0.3f)
{
}

FTimeSeriesChartStyle::FTimeSeriesChartStyle()
    : LineThickness(2.0f)
    , PointRadius(3.0f)
    , bShowPointMarkers(true)
    , bShowXAxis(true)
    , bShowYAxis(true)
    , bShowGridLines(true)
    , bShowLegend(true)
    , bShowTooltips(true)
    , bIncludeZeroInYRange(true)
    , bUseFixedYRange(false)
    , MinimumY(0.0)
    , MaximumY(100.0)
    , YAxisValueMode(EChartAxisValueMode::Auto)
    , XAxisTickCount(5)
    , YAxisTickCount(5)
    , XAxisDateFormat(TEXT("%d %b"))
    , AxisColor(FLinearColor::White)
    , GridLineColor(0.25f, 0.25f, 0.25f, 1.0f)
    , TooltipBackgroundColor(0.03f, 0.03f, 0.04f, 0.96f)
    , TooltipBorderColor(0.35f, 0.35f, 0.8f, 1.0f)
    , TooltipTextColor(FLinearColor::White)
    , UnselectedOpacity(0.3f)
{
}
