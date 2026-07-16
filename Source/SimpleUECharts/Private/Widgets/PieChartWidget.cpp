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
        MyPieChart->SetData(Data);
    }
}

TSharedRef<SWidget> UPieChartWidget::RebuildWidget()
{
    SAssignNew(MyPieChart, SPieChart);
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

#if WITH_EDITOR
const FText UPieChartWidget::GetPaletteCategory()
{
    return LOCTEXT("PaletteCategory", "Simple UE Charts");
}
#endif

#undef LOCTEXT_NAMESPACE
