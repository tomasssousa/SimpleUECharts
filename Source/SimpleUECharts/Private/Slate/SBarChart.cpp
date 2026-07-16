#include "Slate/SBarChart.h"

#include "Framework/Application/SlateApplication.h"
#include "Brushes/SlateColorBrush.h"
#include "Fonts/FontMeasure.h"
#include "Styling/CoreStyle.h"
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

void SBarChart::SetShowLabels(bool bInShowLabels)
{
    bShowLabels = bInShowLabels;
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetShowValues(bool bInShowValues)
{
    bShowValues = bInShowValues;
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetShowYAxis(bool bInShowYAxis)
{
    bShowYAxis = bInShowYAxis;
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetShowGridLines(bool bInShowGridLines)
{
    bShowGridLines = bInShowGridLines;
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

    const float BottomLabelHeight = bShowLabels ? 20.0f : 0.0f;
    const float TopValueHeight = bShowValues ? 18.0f : 0.0f;
    const float AxisLabelWidth = bShowYAxis ? 32.0f : 0.0f;
    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const float PlotLeft = ChartPadding.Left + AxisLabelWidth;
    const float PlotTop = ChartPadding.Top + TopValueHeight;
    const float PlotRight = LocalSize.X - ChartPadding.Right;
    const float PlotBottom = LocalSize.Y - ChartPadding.Bottom - BottomLabelHeight;
    const float PlotWidth = PlotRight - PlotLeft;
    const float PlotHeight = PlotBottom - PlotTop;
    const float AvailableWidth = PlotWidth - (BarSpacing * (Data.Num() - 1));
    const float AvailableHeight = PlotHeight;

    if (PlotWidth <= 0.0f || PlotHeight <= 0.0f || AvailableWidth <= 0.0f || AvailableHeight <= 0.0f)
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

    const int32 GridLayer = LayerId;
    const int32 BarLayer = LayerId + 1;
    const int32 AxisLayer = LayerId + 2;
    const int32 TextLayer = LayerId + 3;
    const ESlateDrawEffect DrawEffects = bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
    const FLinearColor AxisColor = FLinearColor::White * InWidgetStyle.GetColorAndOpacityTint();
    const FLinearColor GridColor = FLinearColor(0.25f, 0.25f, 0.25f, 1.0f) * InWidgetStyle.GetColorAndOpacityTint();
    const FLinearColor TextColor = FLinearColor::White * InWidgetStyle.GetColorAndOpacityTint();
    static const FSlateFontInfo ChartFont = FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 10);
    static const FSlateColorBrush WhiteBrush(FLinearColor::White);

    if (bShowYAxis)
    {
        TArray<FVector2f> AxisPoints;
        AxisPoints.Add(FVector2f(PlotLeft, PlotTop));
        AxisPoints.Add(FVector2f(PlotLeft, PlotBottom));
        AxisPoints.Add(FVector2f(PlotRight, PlotBottom));
        FSlateDrawElement::MakeLines(
            OutDrawElements,
            AxisLayer,
            AllottedGeometry.ToPaintGeometry(),
            MoveTemp(AxisPoints),
            DrawEffects,
            AxisColor,
            true,
            1.0f);
    }

    const int32 TickCount = 4;
    const FNumberFormattingOptions NumberFormat = []()
    {
        FNumberFormattingOptions Options;
        Options.SetMaximumFractionalDigits(2);
        Options.SetMinimumFractionalDigits(0);
        return Options;
    }();

    for (int32 TickIndex = 1; TickIndex <= TickCount; ++TickIndex)
    {
        const float TickAlpha = static_cast<float>(TickIndex) / TickCount;
        const float TickY = PlotBottom - (TickAlpha * PlotHeight);

        if (bShowGridLines)
        {
            TArray<FVector2f> GridPoints;
            GridPoints.Add(FVector2f(PlotLeft, TickY));
            GridPoints.Add(FVector2f(PlotRight, TickY));
            FSlateDrawElement::MakeLines(
                OutDrawElements,
                GridLayer,
                AllottedGeometry.ToPaintGeometry(),
                MoveTemp(GridPoints),
                DrawEffects,
                GridColor,
                true,
                1.0f);
        }

        if (bShowYAxis)
        {
            TArray<FVector2f> TickPoints;
            TickPoints.Add(FVector2f(PlotLeft - 4.0f, TickY));
            TickPoints.Add(FVector2f(PlotLeft, TickY));
            FSlateDrawElement::MakeLines(
                OutDrawElements,
                AxisLayer,
                AllottedGeometry.ToPaintGeometry(),
                MoveTemp(TickPoints),
                DrawEffects,
                AxisColor,
                true,
                1.0f);

            const FText TickText = FText::AsNumber(MaxValue * TickAlpha, &NumberFormat);
            const FVector2D TickTextPosition(ChartPadding.Left, TickY - 8.0f);
            FSlateDrawElement::MakeText(
                OutDrawElements,
                TextLayer,
                AllottedGeometry.ToOffsetPaintGeometry(TickTextPosition),
                TickText,
                ChartFont,
                DrawEffects,
                TextColor);
        }
    }

    for (int32 Index = 0; Index < Data.Num(); ++Index)
    {
        const float NormalizedValue = FMath::Clamp(Data[Index].Value / MaxValue, 0.0f, 1.0f);
        const float BarHeight = NormalizedValue * AvailableHeight;
        const float BarX = PlotLeft + (Index * (BarWidth + BarSpacing));
        const float BarY = PlotBottom - BarHeight;
        const FVector2D BarPosition(BarX, BarY);
        const FVector2D BarSize(BarWidth, BarHeight);

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            BarLayer,
            AllottedGeometry.ToPaintGeometry(BarSize, FSlateLayoutTransform(BarPosition)),
            &WhiteBrush,
            DrawEffects,
            Data[Index].Color * InWidgetStyle.GetColorAndOpacityTint());

        if (bShowValues)
        {
            const FText ValueText = FText::AsNumber(Data[Index].Value, &NumberFormat);
            const FVector2D ValueTextPosition(BarX, FMath::Max(ChartPadding.Top, BarY - 14.0f));
            FSlateDrawElement::MakeText(
                OutDrawElements,
                TextLayer,
                AllottedGeometry.ToOffsetPaintGeometry(ValueTextPosition),
                ValueText,
                ChartFont,
                DrawEffects,
                TextColor);
        }

        if (bShowLabels)
        {
            const FVector2D LabelTextSize = FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(Data[Index].Label, ChartFont);
            const float LabelX = BarX + FMath::Max((BarWidth - LabelTextSize.X) * 0.5f, 0.0f);
            const FVector2D LabelTextPosition(LabelX, PlotBottom + 2.0f);
            FSlateDrawElement::MakeText(
                OutDrawElements,
                TextLayer,
                AllottedGeometry.ToOffsetPaintGeometry(LabelTextPosition),
                Data[Index].Label,
                ChartFont,
                DrawEffects,
                TextColor);
        }
    }

    return TextLayer;
}
