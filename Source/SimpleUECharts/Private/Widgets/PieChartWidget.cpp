#include "Widgets/PieChartWidget.h"

#include "Slate/SPieChart.h"
#include "Widgets/SNullWidget.h"

#define LOCTEXT_NAMESPACE "SimpleUECharts"

void UPieChartWidget::SetData(const TArray<FChartDataPoint>& NewData)
{
    Data = NewData;
    HoveredDataPoint = FChartHoverInfo();
    HoveredDataPointIndex = INDEX_NONE;
    RefreshChart();
}

void UPieChartWidget::ClearData()
{
    Data.Reset();
    HoveredDataPoint = FChartHoverInfo();
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

FChartHoverInfo UPieChartWidget::GetHoveredDataPoint() const
{
    return HoveredDataPoint;
}

TSharedRef<SWidget> UPieChartWidget::RebuildWidget()
{
    HoveredDataPointIndex = INDEX_NONE;
    HoveredDataPoint = FChartHoverInfo();
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
    HoveredDataPoint = FChartHoverInfo();
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
        ResolvedPieChartStyle.HoverOpacityMultiplier = ResolvedPieChartStyle.HoverOpacityMultiplier > KINDA_SMALL_NUMBER
            ? ResolvedPieChartStyle.HoverOpacityMultiplier
            : 1.0f;

        if (ResolvedPieChartStyle.HoverTint.Equals(FLinearColor::Transparent))
        {
            ResolvedPieChartStyle.HoverTint = FLinearColor::White;
        }
        else if (ResolvedPieChartStyle.HoverTint.A <= KINDA_SMALL_NUMBER)
        {
            ResolvedPieChartStyle.HoverTint.A = 1.0f;
        }

        MyPieChart->SetChartStyle(ResolvedChartStyle);
        MyPieChart->SetPieChartStyle(ResolvedPieChartStyle);
    }
}

void UPieChartWidget::HandleSlateHoverChanged(int32 NewHoveredDataPointIndex, FVector2D LocalPosition)
{
    HoveredDataPointIndex = NewHoveredDataPointIndex;

    if (!Data.IsValidIndex(NewHoveredDataPointIndex))
    {
        HoveredDataPoint = FChartHoverInfo();
        OnHoverEnded.Broadcast();
        return;
    }

    float TotalPositiveValue = 0.0f;
    for (const FChartDataPoint& Point : Data)
    {
        if (Point.Value > 0.0f)
        {
            TotalPositiveValue += Point.Value;
        }
    }

    const FChartDataPoint& Point = Data[NewHoveredDataPointIndex];
    HoveredDataPoint = FChartHoverInfo();
    HoveredDataPoint.DataPointIndex = NewHoveredDataPointIndex;
    HoveredDataPoint.Label = Point.Label;
    HoveredDataPoint.Value = Point.Value;
    HoveredDataPoint.Percentage = TotalPositiveValue > 0.0f
        ? Point.Value / TotalPositiveValue
        : 0.0f;
    HoveredDataPoint.LocalPosition = LocalPosition;

    OnDataPointHovered.Broadcast(HoveredDataPoint);
}

#if WITH_EDITOR
const FText UPieChartWidget::GetPaletteCategory()
{
    return LOCTEXT("PaletteCategory", "Simple UE Charts");
}
#endif

#undef LOCTEXT_NAMESPACE
