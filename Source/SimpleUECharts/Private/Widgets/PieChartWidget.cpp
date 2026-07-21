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
    HoveredDataPointIndex = INDEX_NONE;
    RefreshChart();
}

void UPieChartWidget::SetChartStyle(const FChartStyle& NewChartStyle)
{
    ChartStyle = NewChartStyle;
    RefreshChart();
}

void UPieChartWidget::SetPieChartStyle(const FPieChartStyle& NewPieChartStyle)
{
    PieChartStyle = NewPieChartStyle;
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

int32 UPieChartWidget::GetHoveredDataPointIndex() const
{
    return HoveredDataPointIndex;
}

TSharedRef<SWidget> UPieChartWidget::RebuildWidget()
{
    HoveredDataPointIndex = INDEX_NONE;
    SAssignNew(MyPieChart, SPieChart)
        .OnHoveredDataPointChanged(FOnSlateChartHoverChanged::CreateUObject(
            this,
            &UPieChartWidget::HandleSlateHoverChanged));
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
    HoveredDataPointIndex = INDEX_NONE;
}

void UPieChartWidget::SynchronizePieChartProperties()
{
    if (MyPieChart.IsValid())
    {
        FChartStyle ResolvedChartStyle = ChartStyle;
        ResolvedChartStyle.ChartScale = FMath::Max(0.1f, ResolvedChartStyle.ChartScale);
        ResolvedChartStyle.TextScale = FMath::Max(0.1f, ResolvedChartStyle.TextScale);

        FPieChartStyle ResolvedPieChartStyle = PieChartStyle;
        ResolvedPieChartStyle.SliceSpacing = FMath::Max(0.0f, ResolvedPieChartStyle.SliceSpacing);
        ResolvedPieChartStyle.InnerRadius = FMath::Clamp(ResolvedPieChartStyle.InnerRadius, 0.0f, 0.95f);
        ResolvedPieChartStyle.PieHorizontalAlignment = FMath::Clamp(ResolvedPieChartStyle.PieHorizontalAlignment, 0.0f, 1.0f);
        ResolvedPieChartStyle.LegendSpacing = FMath::Max(0.0f, ResolvedPieChartStyle.LegendSpacing);

        MyPieChart->SetChartStyle(ResolvedChartStyle);
        MyPieChart->SetPieChartStyle(ResolvedPieChartStyle);
    }
}

void UPieChartWidget::HandleSlateHoverChanged(int32 NewHoveredDataPointIndex)
{
    HoveredDataPointIndex = NewHoveredDataPointIndex;
}

#if WITH_EDITOR
const FText UPieChartWidget::GetPaletteCategory()
{
    return LOCTEXT("PaletteCategory", "Simple UE Charts");
}
#endif

#undef LOCTEXT_NAMESPACE
