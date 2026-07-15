#include "Slate/SBarChart.h"

void SBarChart::Construct(const FArguments& InArgs)
{
}

void SBarChart::SetData(const TArray<FChartDataPoint>& NewData)
{
    Data = NewData;
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::ClearData()
{
    Data.Reset();
    Invalidate(EInvalidateWidgetReason::Paint);
}

FVector2D SBarChart::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return FVector2D(300.0f, 200.0f);
}

int32 SBarChart::OnPaint(
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
