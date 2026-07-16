#include "Widgets/PieChartWidget.h"

#include "Slate/SPieChart.h"
#include "Widgets/SNullWidget.h"

#define LOCTEXT_NAMESPACE "SimpleUECharts"

void UPieChartWidget::SetData(const TArray<FChartDataPoint>& NewData)
{
    Data = NewData;
    RefreshChart();
}

void UPieChartWidget::ClearData()
{
    Data.Reset();
    RefreshChart();
}

void UPieChartWidget::RefreshChart()
{
    if (MyPieChart.IsValid())
    {
        SynchronizePieChartProperties();
        MyPieChart->SetData(Data);
    }
}

TSharedRef<SWidget> UPieChartWidget::RebuildWidget()
{
    SAssignNew(MyPieChart, SPieChart);
    SynchronizePieChartProperties();
    MyPieChart->SetData(Data);

    return MyPieChart.IsValid() ? MyPieChart.ToSharedRef() : SNullWidget::NullWidget;
}

void UPieChartWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    RefreshChart();
}

void UPieChartWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    MyPieChart.Reset();
}

void UPieChartWidget::SynchronizePieChartProperties()
{
    if (MyPieChart.IsValid())
    {
        MyPieChart->SetShowLabels(bShowLabels);
        MyPieChart->SetShowValues(bShowValues);
        MyPieChart->SetShowPercentages(bShowPercentages);
        MyPieChart->SetShowLegend(bShowLegend);
        MyPieChart->SetStartAngle(StartAngle);
        MyPieChart->SetSliceSpacing(SliceSpacing);
        MyPieChart->SetInnerRadius(InnerRadius);
    }
}

#if WITH_EDITOR
const FText UPieChartWidget::GetPaletteCategory()
{
    return LOCTEXT("PaletteCategory", "Simple UE Charts");
}
#endif

#undef LOCTEXT_NAMESPACE
