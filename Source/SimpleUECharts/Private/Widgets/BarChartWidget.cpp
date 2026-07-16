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

void UBarChartWidget::SetBarSpacing(float NewBarSpacing)
{
    BarSpacing = FMath::Max(0.0f, NewBarSpacing);
    RefreshChart();
}

void UBarChartWidget::SetChartPadding(FMargin NewChartPadding)
{
    ChartPadding = NewChartPadding;
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
        MyBarChart->SetBarSpacing(BarSpacing);
        MyBarChart->SetChartPadding(ChartPadding);
        MyBarChart->SetShowLabels(bShowLabels);
        MyBarChart->SetShowValues(bShowValues);
        MyBarChart->SetShowYAxis(bShowYAxis);
        MyBarChart->SetShowGridLines(bShowGridLines);
    }
}

#if WITH_EDITOR
const FText UBarChartWidget::GetPaletteCategory()
{
    return LOCTEXT("PaletteCategory", "Simple UE Charts");
}
#endif

#undef LOCTEXT_NAMESPACE
