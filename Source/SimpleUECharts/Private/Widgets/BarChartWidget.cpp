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

TSharedRef<SWidget> UBarChartWidget::RebuildWidget()
{
    SAssignNew(MyBarChart, SBarChart);
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
}

void UBarChartWidget::SynchronizeBarChartProperties()
{
    if (MyBarChart.IsValid())
    {
        FBarChartStyle ResolvedBarChartStyle = BarChartStyle;
        ResolvedBarChartStyle.BarSpacing = FMath::Max(0.0f, ResolvedBarChartStyle.BarSpacing);
        ResolvedBarChartStyle.MinimumBarWidth = FMath::Max(0.0f, ResolvedBarChartStyle.MinimumBarWidth);
        ResolvedBarChartStyle.MaximumBarWidth = FMath::Max(0.0f, ResolvedBarChartStyle.MaximumBarWidth);

        if (ResolvedBarChartStyle.MaximumBarWidth > 0.0f &&
            ResolvedBarChartStyle.MinimumBarWidth > ResolvedBarChartStyle.MaximumBarWidth)
        {
            Swap(ResolvedBarChartStyle.MinimumBarWidth, ResolvedBarChartStyle.MaximumBarWidth);
        }

        MyBarChart->SetChartStyle(ChartStyle);
        MyBarChart->SetBarChartStyle(ResolvedBarChartStyle);
    }
}

#if WITH_EDITOR
const FText UBarChartWidget::GetPaletteCategory()
{
    return LOCTEXT("PaletteCategory", "Simple UE Charts");
}
#endif

#undef LOCTEXT_NAMESPACE
