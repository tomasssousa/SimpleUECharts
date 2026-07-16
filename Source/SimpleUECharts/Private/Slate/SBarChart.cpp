#include "Slate/SBarChart.h"

#include "Framework/Application/SlateApplication.h"
#include "Brushes/SlateColorBrush.h"
#include "Fonts/FontMeasure.h"
#include "Styling/CoreStyle.h"
#include "Rendering/DrawElements.h"

void SBarChart::Construct(const FArguments& InArgs)
{
    RecalculateChart();
}

void SBarChart::SetData(const TArray<FChartDataPoint>& NewData)
{
    Data = NewData;
    RecalculateChart();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::ClearData()
{
    Data.Reset();
    RecalculateChart();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetBarSpacing(float InBarSpacing)
{
    BarSpacing = FMath::Max(0.0f, InBarSpacing);
    InvalidateCachedLayout();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetChartPadding(const FMargin& InChartPadding)
{
    ChartPadding = InChartPadding;
    InvalidateCachedLayout();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetShowLabels(bool bInShowLabels)
{
    bShowLabels = bInShowLabels;
    RecalculateChart();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetShowValues(bool bInShowValues)
{
    bShowValues = bInShowValues;
    RecalculateChart();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetShowYAxis(bool bInShowYAxis)
{
    bShowYAxis = bInShowYAxis;
    InvalidateCachedLayout();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetShowGridLines(bool bInShowGridLines)
{
    bShowGridLines = bInShowGridLines;
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::RecalculateChart()
{
    CachedBars.Reset();
    CachedMaxValue = 0.0f;

    const FNumberFormattingOptions NumberFormat = []()
    {
        FNumberFormattingOptions Options;
        Options.SetMaximumFractionalDigits(2);
        Options.SetMinimumFractionalDigits(0);
        return Options;
    }();

    for (const FChartDataPoint& Point : Data)
    {
        CachedMaxValue = FMath::Max(CachedMaxValue, Point.Value);
    }

    CachedBars.Reserve(Data.Num());
    for (const FChartDataPoint& Point : Data)
    {
        FCachedBarData& CachedBar = CachedBars.AddDefaulted_GetRef();
        CachedBar.Label = Point.Label;
        CachedBar.Value = Point.Value;
        CachedBar.ValueText = bShowValues ? FText::AsNumber(Point.Value, &NumberFormat) : FText::GetEmpty();
        CachedBar.NormalizedValue = CachedMaxValue > 0.0f
            ? FMath::Clamp(Point.Value / CachedMaxValue, 0.0f, 1.0f)
            : 0.0f;
        CachedBar.Color = Point.Color;
    }

    InvalidateCachedLayout();
}

void SBarChart::InvalidateCachedLayout()
{
    bLayoutDirty = true;
    CachedLocalSize = FVector2D(-1.0f, -1.0f);
    CachedBarLayouts.Reset();
    CachedTickLayouts.Reset();
}

void SBarChart::EnsureCachedLayout(const FVector2D& LocalSize) const
{
    if (!bLayoutDirty && CachedLocalSize.Equals(LocalSize, KINDA_SMALL_NUMBER))
    {
        return;
    }

    bLayoutDirty = false;
    CachedLocalSize = LocalSize;
    CachedBarLayouts.Reset();
    CachedTickLayouts.Reset();
    CachedPlotLeft = 0.0f;
    CachedPlotTop = 0.0f;
    CachedPlotRight = 0.0f;
    CachedPlotBottom = 0.0f;

    if (CachedBars.IsEmpty() || CachedMaxValue <= 0.0f)
    {
        return;
    }

    const float BottomLabelHeight = bShowLabels ? 20.0f : 0.0f;
    const float TopValueHeight = bShowValues ? 18.0f : 0.0f;
    const float AxisLabelWidth = bShowYAxis ? 32.0f : 0.0f;
    const float PlotLeft = ChartPadding.Left + AxisLabelWidth;
    const float PlotTop = ChartPadding.Top + TopValueHeight;
    const float PlotRight = LocalSize.X - ChartPadding.Right;
    const float PlotBottom = LocalSize.Y - ChartPadding.Bottom - BottomLabelHeight;
    const float PlotWidth = PlotRight - PlotLeft;
    const float PlotHeight = PlotBottom - PlotTop;
    const float AvailableWidth = PlotWidth - (BarSpacing * (CachedBars.Num() - 1));
    const float AvailableHeight = PlotHeight;

    if (PlotWidth <= 0.0f || PlotHeight <= 0.0f || AvailableWidth <= 0.0f || AvailableHeight <= 0.0f)
    {
        return;
    }

    const float BarWidth = AvailableWidth / CachedBars.Num();
    if (BarWidth <= 0.0f)
    {
        return;
    }

    CachedPlotLeft = PlotLeft;
    CachedPlotTop = PlotTop;
    CachedPlotRight = PlotRight;
    CachedPlotBottom = PlotBottom;

    static const FSlateFontInfo ChartFont = FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 10);
    const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

    const int32 TickCount = 4;
    CachedTickLayouts.Reserve(TickCount);
    for (int32 TickIndex = 1; TickIndex <= TickCount; ++TickIndex)
    {
        const float TickAlpha = static_cast<float>(TickIndex) / TickCount;
        FCachedTickLayout& TickLayout = CachedTickLayouts.AddDefaulted_GetRef();
        TickLayout.Y = PlotBottom - (TickAlpha * PlotHeight);
        TickLayout.Text = FText::AsNumber(CachedMaxValue * TickAlpha);
        TickLayout.TextPosition = FVector2D(ChartPadding.Left, TickLayout.Y - 8.0f);
    }

    CachedBarLayouts.Reserve(CachedBars.Num());
    for (int32 Index = 0; Index < CachedBars.Num(); ++Index)
    {
        const FCachedBarData& CachedBar = CachedBars[Index];
        const float BarHeight = CachedBar.NormalizedValue * AvailableHeight;
        const float BarX = PlotLeft + (Index * (BarWidth + BarSpacing));
        const float BarY = PlotBottom - BarHeight;

        FCachedBarLayout& BarLayout = CachedBarLayouts.AddDefaulted_GetRef();
        BarLayout.Position = FVector2D(BarX, BarY);
        BarLayout.Size = FVector2D(BarWidth, BarHeight);
        BarLayout.ValueTextPosition = FVector2D(BarX, FMath::Max(ChartPadding.Top, BarY - 14.0f));

        const FVector2D LabelTextSize = FontMeasure->Measure(CachedBar.Label, ChartFont);
        const float LabelX = BarX + FMath::Max((BarWidth - LabelTextSize.X) * 0.5f, 0.0f);
        BarLayout.LabelTextPosition = FVector2D(LabelX, PlotBottom + 2.0f);
    }
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
    if (CachedBars.IsEmpty())
    {
        return LayerId;
    }

    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    EnsureCachedLayout(LocalSize);

    if (CachedBarLayouts.IsEmpty())
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
        AxisPoints.Add(FVector2f(CachedPlotLeft, CachedPlotTop));
        AxisPoints.Add(FVector2f(CachedPlotLeft, CachedPlotBottom));
        AxisPoints.Add(FVector2f(CachedPlotRight, CachedPlotBottom));
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

    for (const FCachedTickLayout& TickLayout : CachedTickLayouts)
    {
        if (bShowGridLines)
        {
            TArray<FVector2f> GridPoints;
            GridPoints.Add(FVector2f(CachedPlotLeft, TickLayout.Y));
            GridPoints.Add(FVector2f(CachedPlotRight, TickLayout.Y));
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
            TickPoints.Add(FVector2f(CachedPlotLeft - 4.0f, TickLayout.Y));
            TickPoints.Add(FVector2f(CachedPlotLeft, TickLayout.Y));
            FSlateDrawElement::MakeLines(
                OutDrawElements,
                AxisLayer,
                AllottedGeometry.ToPaintGeometry(),
                MoveTemp(TickPoints),
                DrawEffects,
                AxisColor,
                true,
                1.0f);

            FSlateDrawElement::MakeText(
                OutDrawElements,
                TextLayer,
                AllottedGeometry.ToOffsetPaintGeometry(TickLayout.TextPosition),
                TickLayout.Text,
                ChartFont,
                DrawEffects,
                TextColor);
        }
    }

    for (int32 Index = 0; Index < CachedBars.Num(); ++Index)
    {
        const FCachedBarData& CachedBar = CachedBars[Index];
        const FCachedBarLayout& BarLayout = CachedBarLayouts[Index];

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            BarLayer,
            AllottedGeometry.ToPaintGeometry(BarLayout.Size, FSlateLayoutTransform(BarLayout.Position)),
            &WhiteBrush,
            DrawEffects,
            CachedBar.Color * InWidgetStyle.GetColorAndOpacityTint());

        if (bShowValues)
        {
            FSlateDrawElement::MakeText(
                OutDrawElements,
                TextLayer,
                AllottedGeometry.ToOffsetPaintGeometry(BarLayout.ValueTextPosition),
                CachedBar.ValueText,
                ChartFont,
                DrawEffects,
                TextColor);
        }

        if (bShowLabels)
        {
            FSlateDrawElement::MakeText(
                OutDrawElements,
                TextLayer,
                AllottedGeometry.ToOffsetPaintGeometry(BarLayout.LabelTextPosition),
                CachedBar.Label,
                ChartFont,
                DrawEffects,
                TextColor);
        }
    }

    return TextLayer;
}
