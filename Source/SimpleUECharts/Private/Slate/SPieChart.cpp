#include "Slate/SPieChart.h"

void SPieChart::Construct(const FArguments& InArgs)
{
}

void SPieChart::SetData(const TArray<FChartDataPoint>& NewData)
{
    Data = NewData;
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::ClearData()
{
    Data.Reset();
    Invalidate(EInvalidateWidgetReason::Paint);
}

FVector2D SPieChart::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return FVector2D(300.0f, 200.0f);
}

int32 SPieChart::OnPaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled) const
{
    // Rendering logic will be implemented in a later phase of the roadmap.
    return LayerId;
}
