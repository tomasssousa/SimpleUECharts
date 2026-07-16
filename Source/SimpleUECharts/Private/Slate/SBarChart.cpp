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
    const float HorizontalPadding = 8.0f;
    const float VerticalPadding = 8.0f;
    const float BarSpacing = 6.0f;
    const float AvailableWidth = LocalSize.X - (HorizontalPadding * 2.0f) - (BarSpacing * (Data.Num() - 1));
    const float AvailableHeight = LocalSize.Y - (VerticalPadding * 2.0f);

    if (AvailableWidth <= 0.0f || AvailableHeight <= 0.0f)
    {
        return LayerId;
    }

    const float BarWidth = AvailableWidth / Data.Num();
    const float BarHeight = FMath::Max(AvailableHeight * 0.7f, 1.0f);
    const float BarY = LocalSize.Y - VerticalPadding - BarHeight;
    const ESlateDrawEffect DrawEffects = bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
    static const FSlateColorBrush WhiteBrush(FLinearColor::White);

    for (int32 Index = 0; Index < Data.Num(); ++Index)
    {
        const float BarX = HorizontalPadding + (Index * (BarWidth + BarSpacing));
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
