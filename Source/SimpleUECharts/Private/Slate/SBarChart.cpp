#include "Slate/SBarChart.h"

#include "Brushes/SlateColorBrush.h"
#include "Rendering/DrawElements.h"

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

void SBarChart::SetBarSpacing(float InBarSpacing)
{
    BarSpacing = FMath::Max(0.0f, InBarSpacing);
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetChartPadding(const FMargin& InChartPadding)
{
    ChartPadding = InChartPadding;
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
    if (Data.IsEmpty())
    {
        return LayerId;
    }

    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const float AvailableWidth = LocalSize.X - ChartPadding.Left - ChartPadding.Right - (BarSpacing * (Data.Num() - 1));
    const float AvailableHeight = LocalSize.Y - ChartPadding.Top - ChartPadding.Bottom;

    if (AvailableWidth <= 0.0f || AvailableHeight <= 0.0f)
    {
        return LayerId;
    }

    float MaxValue = 0.0f;
    for (const FChartDataPoint& Point : Data)
    {
        MaxValue = FMath::Max(MaxValue, Point.Value);
    }

    if (MaxValue <= 0.0f)
    {
        return LayerId;
    }

    const float BarWidth = AvailableWidth / Data.Num();

    if (BarWidth <= 0.0f)
    {
        return LayerId;
    }

    const ESlateDrawEffect DrawEffects = bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
    static const FSlateColorBrush WhiteBrush(FLinearColor::White);

    for (int32 Index = 0; Index < Data.Num(); ++Index)
    {
        const float NormalizedValue = FMath::Clamp(Data[Index].Value / MaxValue, 0.0f, 1.0f);
        const float BarHeight = NormalizedValue * AvailableHeight;
        const float BarX = ChartPadding.Left + (Index * (BarWidth + BarSpacing));
        const float BarY = LocalSize.Y - ChartPadding.Bottom - BarHeight;
        const FVector2D BarPosition(BarX, BarY);
        const FVector2D BarSize(BarWidth, BarHeight);

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(BarSize, FSlateLayoutTransform(BarPosition)),
            &WhiteBrush,
            DrawEffects,
            Data[Index].Color * InWidgetStyle.GetColorAndOpacityTint());
    }

    return LayerId;
}
