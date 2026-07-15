#include "Widgets/BarChartWidget.h"

#include "Slate/SBarChart.h"
#include "Widgets/SNullWidget.h"

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

void UBarChartWidget::RefreshChart()
{
    if (MyBarChart.IsValid())
    {
        MyBarChart->SetData(Data);
    }
}

TSharedRef<SWidget> UBarChartWidget::RebuildWidget()
{
    SAssignNew(MyBarChart, SBarChart);
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
