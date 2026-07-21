#include "Widgets/BarChartWidget.h"

#include "Slate/SBarChart.h"
#include "Widgets/SNullWidget.h"

#define LOCTEXT_NAMESPACE "SimpleUECharts"

void UBarChartWidget::SetData(const TArray<FChartDataPoint>& NewData)
{
    Data = NewData;
    RefreshChart();
}

void UBarChartWidget::ClearData()
{
    Data.Reset();
    HoveredDataPointIndex = INDEX_NONE;
    RefreshChart();
}

void UBarChartWidget::SetChartStyle(const FChartStyle& NewChartStyle)
{
    ChartStyle = NewChartStyle;
    RefreshChart();
}

void UBarChartWidget::SetBarChartStyle(const FBarChartStyle& NewBarChartStyle)
{
    BarChartStyle = NewBarChartStyle;
    RefreshChart();
}

void UBarChartWidget::SetBarSpacing(float NewBarSpacing)
{
    BarChartStyle.BarSpacing = FMath::Max(0.0f, NewBarSpacing);
    RefreshChart();
}

void UBarChartWidget::SetChartPadding(FMargin NewChartPadding)
{
    ChartStyle.Padding = NewChartPadding;
    RefreshChart();
}

void UBarChartWidget::RefreshChart()
{
    if (MyBarChart.IsValid())
    {
        SynchronizeBarChartProperties();
        MyBarChart->SetData(Data);
    }
}

int32 UBarChartWidget::GetHoveredDataPointIndex() const
{
    return HoveredDataPointIndex;
}

TSharedRef<SWidget> UBarChartWidget::RebuildWidget()
{
    HoveredDataPointIndex = INDEX_NONE;
    SAssignNew(MyBarChart, SBarChart)
        .OnHoveredDataPointChanged(FOnSlateChartHoverChanged::CreateUObject(
            this,
            &UBarChartWidget::HandleSlateHoverChanged));
    SynchronizeBarChartProperties();
    MyBarChart->SetData(Data);

    return MyBarChart.IsValid() ? MyBarChart.ToSharedRef() : SNullWidget::NullWidget;
}

void UBarChartWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    RefreshChart();
}

void UBarChartWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    MyBarChart.Reset();
    HoveredDataPointIndex = INDEX_NONE;
}

void UBarChartWidget::SynchronizeBarChartProperties()
{
    if (MyBarChart.IsValid())
    {
        FChartStyle ResolvedChartStyle = ChartStyle;
        ResolvedChartStyle.ChartScale = FMath::Max(0.1f, ResolvedChartStyle.ChartScale);
        ResolvedChartStyle.TextScale = FMath::Max(0.1f, ResolvedChartStyle.TextScale);

        FBarChartStyle ResolvedBarChartStyle = BarChartStyle;
        ResolvedBarChartStyle.BarSpacing = FMath::Max(0.0f, ResolvedBarChartStyle.BarSpacing);
        ResolvedBarChartStyle.MinimumBarWidth = FMath::Max(0.0f, ResolvedBarChartStyle.MinimumBarWidth);
        ResolvedBarChartStyle.MaximumBarWidth = FMath::Max(0.0f, ResolvedBarChartStyle.MaximumBarWidth);

        if (ResolvedBarChartStyle.MaximumBarWidth > 0.0f &&
            ResolvedBarChartStyle.MinimumBarWidth > ResolvedBarChartStyle.MaximumBarWidth)
        {
            Swap(ResolvedBarChartStyle.MinimumBarWidth, ResolvedBarChartStyle.MaximumBarWidth);
        }

        MyBarChart->SetChartStyle(ResolvedChartStyle);
        MyBarChart->SetBarChartStyle(ResolvedBarChartStyle);
    }
}

void UBarChartWidget::HandleSlateHoverChanged(int32 NewHoveredDataPointIndex)
{
    HoveredDataPointIndex = NewHoveredDataPointIndex;
}

#if WITH_EDITOR
const FText UBarChartWidget::GetPaletteCategory()
{
    return LOCTEXT("PaletteCategory", "Simple UE Charts");
}
#endif

#undef LOCTEXT_NAMESPACE
