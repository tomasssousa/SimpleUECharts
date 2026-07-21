#include "Slate/SBarChart.h"

#include "Framework/Application/SlateApplication.h"
#include "Brushes/SlateColorBrush.h"
#include "Fonts/FontMeasure.h"
#include "Styling/CoreStyle.h"
#include "Rendering/DrawElements.h"

namespace
{
float GetChartScale(const FChartStyle& InChartStyle)
{
    return FMath::Max(0.1f, InChartStyle.ChartScale);
}

float GetTextScale(const FChartStyle& InChartStyle)
{
    return FMath::Max(0.1f, InChartStyle.TextScale);
}

FMargin ScaleMargin(const FMargin& InMargin, float Scale)
{
    return FMargin(
        InMargin.Left * Scale,
        InMargin.Top * Scale,
        InMargin.Right * Scale,
        InMargin.Bottom * Scale);
}
}

void SBarChart::Construct(const FArguments& InArgs)
{
    OnHoveredDataPointChanged = InArgs._OnHoveredDataPointChanged;
    RecalculateChart();
}

void SBarChart::SetData(const TArray<FChartDataPoint>& NewData)
{
    Data = NewData;
    ClearHover();
    RecalculateChart();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::ClearData()
{
    Data.Reset();
    ClearHover();
    RecalculateChart();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetChartStyle(const FChartStyle& InChartStyle)
{
    ChartStyle = InChartStyle;
    ClearHover();
    RecalculateChart();
    InvalidateCachedLayout();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetBarChartStyle(const FBarChartStyle& InBarChartStyle)
{
    BarChartStyle = InBarChartStyle;
    ClearHover();
    RecalculateChart();
    InvalidateCachedLayout();
    Invalidate(EInvalidateWidgetReason::Paint);
}

int32 SBarChart::GetHoveredDataPointIndex() const
{
    return HoveredDataPointIndex;
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
        CachedBar.ValueText = BarChartStyle.bShowValues
            ? FText::AsNumber(Point.Value, &NumberFormat)
            : FText::GetEmpty();
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

    const float ChartScale = GetChartScale(ChartStyle);
    const float TextScale = GetTextScale(ChartStyle);
    const FMargin ScaledLabelPadding = ScaleMargin(ChartStyle.LabelPadding, TextScale);
    const FSlateFontInfo ChartFont = GetChartFont();
    const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
    const float FontHeight = FontMeasure->GetMaxCharacterHeight(ChartFont);
    const float LabelSpacing = ScaledLabelPadding.Top + ScaledLabelPadding.Bottom;
    const float ValueSpacing = ScaledLabelPadding.Top + ScaledLabelPadding.Bottom;
    const float BottomLabelHeight = BarChartStyle.bShowLabels ? FontHeight + LabelSpacing : 0.0f;
    const float TopValueHeight = BarChartStyle.bShowValues ? FontHeight + ValueSpacing : 0.0f;
    const float TickMarkLength = BarChartStyle.bShowYAxis ? 4.0f * ChartScale : 0.0f;
    const int32 TickCount = 4;
    TArray<FText, TInlineAllocator<4>> TickTexts;
    TArray<FVector2D, TInlineAllocator<4>> TickTextSizes;
    float MaxTickTextWidth = 0.0f;

    if (BarChartStyle.bShowYAxis)
    {
        TickTexts.Reserve(TickCount);
        TickTextSizes.Reserve(TickCount);

        for (int32 TickIndex = 1; TickIndex <= TickCount; ++TickIndex)
        {
            const float TickAlpha = static_cast<float>(TickIndex) / TickCount;
            const FText TickText = FText::AsNumber(CachedMaxValue * TickAlpha);
            const FVector2D TickTextSize = FontMeasure->Measure(TickText, ChartFont);
            TickTexts.Add(TickText);
            TickTextSizes.Add(TickTextSize);
            MaxTickTextWidth = FMath::Max(MaxTickTextWidth, TickTextSize.X);
        }
    }

    const float AxisLabelWidth = BarChartStyle.bShowYAxis
        ? MaxTickTextWidth + TickMarkLength + ScaledLabelPadding.Left + ScaledLabelPadding.Right
        : 0.0f;
    const float PlotLeft = ChartStyle.Padding.Left + AxisLabelWidth;
    const float PlotTop = ChartStyle.Padding.Top + TopValueHeight;
    const float PlotRight = LocalSize.X - ChartStyle.Padding.Right;
    const float PlotBottom = LocalSize.Y - ChartStyle.Padding.Bottom - BottomLabelHeight;
    const float AvailablePlotWidth = PlotRight - PlotLeft;
    const float AvailablePlotHeight = PlotBottom - PlotTop;
    const float PlotWidth = AvailablePlotWidth * FMath::Min(ChartScale, 1.0f);
    const float PlotHeight = AvailablePlotHeight * FMath::Min(ChartScale, 1.0f);
    const float PlotOffsetX = FMath::Max((AvailablePlotWidth - PlotWidth) * 0.5f, 0.0f);
    const float ScaledPlotLeft = PlotLeft + PlotOffsetX;
    const float ScaledPlotRight = ScaledPlotLeft + PlotWidth;
    const float ScaledPlotTop = PlotBottom - PlotHeight;
    const float ScaledBarSpacing = BarChartStyle.BarSpacing * ChartScale;
    const float AvailableWidth = PlotWidth - (ScaledBarSpacing * (CachedBars.Num() - 1));
    const float AvailableHeight = PlotHeight;

    if (PlotWidth <= 0.0f || PlotHeight <= 0.0f || AvailableWidth <= 0.0f || AvailableHeight <= 0.0f)
    {
        return;
    }

    const float BaseBarWidth = AvailableWidth / CachedBars.Num();
    const float DesiredMinimumBarWidth = FMath::Max(0.0f, BarChartStyle.MinimumBarWidth * ChartScale);
    const float DesiredMaximumBarWidth = BarChartStyle.MaximumBarWidth > 0.0f
        ? FMath::Max(DesiredMinimumBarWidth, BarChartStyle.MaximumBarWidth * ChartScale)
        : 0.0f;

    float BarWidth = BaseBarWidth;
    if (DesiredMaximumBarWidth > 0.0f)
    {
        BarWidth = FMath::Min(BarWidth, DesiredMaximumBarWidth);
    }

    if (DesiredMinimumBarWidth > 0.0f)
    {
        const float MinimumWidthRequirement =
            (DesiredMinimumBarWidth * CachedBars.Num()) +
            (ScaledBarSpacing * (CachedBars.Num() - 1));

        if (MinimumWidthRequirement <= PlotWidth)
        {
            BarWidth = FMath::Max(BarWidth, DesiredMinimumBarWidth);
        }
    }

    if (BarWidth <= 0.0f)
    {
        return;
    }

    const float OccupiedWidth =
        (BarWidth * CachedBars.Num()) +
        (ScaledBarSpacing * (CachedBars.Num() - 1));
    const float BarStartX = ScaledPlotLeft + FMath::Max((PlotWidth - OccupiedWidth) * 0.5f, 0.0f);

    CachedPlotLeft = ScaledPlotLeft;
    CachedPlotTop = ScaledPlotTop;
    CachedPlotRight = ScaledPlotRight;
    CachedPlotBottom = PlotBottom;

    CachedTickLayouts.Reserve(TickCount);
    for (int32 TickIndex = 1; TickIndex <= TickCount; ++TickIndex)
    {
        const float TickAlpha = static_cast<float>(TickIndex) / TickCount;
        FCachedTickLayout& TickLayout = CachedTickLayouts.AddDefaulted_GetRef();
        TickLayout.Y = PlotBottom - (TickAlpha * PlotHeight);
        TickLayout.Text = BarChartStyle.bShowYAxis ? TickTexts[TickIndex - 1] : FText::GetEmpty();

        if (BarChartStyle.bShowYAxis)
        {
            const FVector2D& TickTextSize = TickTextSizes[TickIndex - 1];
            TickLayout.TextPosition = FVector2D(
                ChartStyle.Padding.Left + ScaledLabelPadding.Left + FMath::Max(MaxTickTextWidth - TickTextSize.X, 0.0f),
                TickLayout.Y - (TickTextSize.Y * 0.5f));
        }
    }

    CachedBarLayouts.Reserve(CachedBars.Num());
    for (int32 Index = 0; Index < CachedBars.Num(); ++Index)
    {
        const FCachedBarData& CachedBar = CachedBars[Index];
        const float BarHeight = CachedBar.NormalizedValue * AvailableHeight;
        const float BarX = BarStartX + (Index * (BarWidth + ScaledBarSpacing));
        const float BarY = PlotBottom - BarHeight;

        FCachedBarLayout& BarLayout = CachedBarLayouts.AddDefaulted_GetRef();
        BarLayout.Position = FVector2D(BarX, BarY);
        BarLayout.Size = FVector2D(BarWidth, BarHeight);

        const FVector2D ValueTextSize = FontMeasure->Measure(CachedBar.ValueText, ChartFont);
        const float ValueX = BarX + FMath::Max((BarWidth - ValueTextSize.X) * 0.5f, 0.0f);
        const float ValueY = FMath::Max(
            ChartStyle.Padding.Top,
            BarY - ValueTextSize.Y - ScaledLabelPadding.Bottom);
        BarLayout.ValueTextPosition = FVector2D(ValueX, ValueY);

        const FVector2D LabelTextSize = FontMeasure->Measure(CachedBar.Label, ChartFont);
        const float LabelX = BarX + FMath::Max((BarWidth - LabelTextSize.X) * 0.5f, 0.0f);
        BarLayout.LabelTextPosition = FVector2D(LabelX, PlotBottom + ScaledLabelPadding.Top);
    }
}

FSlateFontInfo SBarChart::GetChartFont() const
{
    FSlateFontInfo ChartFont = ChartStyle.Font.Size > 0
        ? ChartStyle.Font
        : FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 10);

    ChartFont.Size = FMath::Max(1, FMath::RoundToInt(ChartFont.Size * GetTextScale(ChartStyle)));
    return ChartFont;
}

int32 SBarChart::FindHoveredDataPointIndex(const FVector2D& LocalPosition) const
{
    return INDEX_NONE;
}

void SBarChart::SetHoveredDataPointIndex(int32 NewHoveredDataPointIndex)
{
    if (HoveredDataPointIndex == NewHoveredDataPointIndex)
    {
        return;
    }

    HoveredDataPointIndex = NewHoveredDataPointIndex;
    OnHoveredDataPointChanged.ExecuteIfBound(HoveredDataPointIndex);
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::ClearHover()
{
    SetHoveredDataPointIndex(INDEX_NONE);
}

FVector2D SBarChart::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    const float ChartScale = GetChartScale(ChartStyle);
    const float TextScale = GetTextScale(ChartStyle);

    return FVector2D(
        (220.0f * ChartScale) + (80.0f * TextScale),
        (120.0f * ChartScale) + (80.0f * TextScale));
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
    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const float ChartScale = GetChartScale(ChartStyle);
    const float AxisLineThickness = FMath::Max(1.0f, ChartScale);
    const float TickMarkLength = 4.0f * ChartScale;
    const bool bHasValidBackground = ChartStyle.BackgroundColor.A > 0.0f;
    const int32 BackgroundLayer = LayerId;
    const int32 GridLayer = LayerId + (bHasValidBackground ? 1 : 0);
    const int32 BarLayer = GridLayer + 1;
    const int32 AxisLayer = GridLayer + 2;
    const int32 TextLayer = GridLayer + 3;
    const ESlateDrawEffect DrawEffects = bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
    const FLinearColor AxisColor = BarChartStyle.AxisColor * InWidgetStyle.GetColorAndOpacityTint();
    const FLinearColor GridColor = BarChartStyle.GridLineColor * InWidgetStyle.GetColorAndOpacityTint();
    const FLinearColor TextColor = ChartStyle.TextColor * InWidgetStyle.GetColorAndOpacityTint();
    const FLinearColor BackgroundColor = ChartStyle.BackgroundColor * InWidgetStyle.GetColorAndOpacityTint();
    const FSlateFontInfo ChartFont = GetChartFont();
    static const FSlateColorBrush WhiteBrush(FLinearColor::White);

    if (bHasValidBackground)
    {
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            BackgroundLayer,
            AllottedGeometry.ToPaintGeometry(LocalSize, FSlateLayoutTransform()),
            &WhiteBrush,
            DrawEffects,
            BackgroundColor);
    }

    if (CachedBars.IsEmpty())
    {
        return bHasValidBackground ? BackgroundLayer : LayerId;
    }

    EnsureCachedLayout(LocalSize);

    if (CachedBarLayouts.IsEmpty())
    {
        return bHasValidBackground ? BackgroundLayer : LayerId;
    }

    if (BarChartStyle.bShowYAxis)
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
            AxisLineThickness);
    }

    for (const FCachedTickLayout& TickLayout : CachedTickLayouts)
    {
        if (BarChartStyle.bShowGridLines)
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
                AxisLineThickness);
        }

        if (BarChartStyle.bShowYAxis)
        {
            TArray<FVector2f> TickPoints;
            TickPoints.Add(FVector2f(CachedPlotLeft - TickMarkLength, TickLayout.Y));
            TickPoints.Add(FVector2f(CachedPlotLeft, TickLayout.Y));
            FSlateDrawElement::MakeLines(
                OutDrawElements,
                AxisLayer,
                AllottedGeometry.ToPaintGeometry(),
                MoveTemp(TickPoints),
                DrawEffects,
                AxisColor,
                true,
                AxisLineThickness);

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

        if (BarChartStyle.bShowValues)
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

        if (BarChartStyle.bShowLabels)
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

FReply SBarChart::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    const FVector2D LocalPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
    SetHoveredDataPointIndex(FindHoveredDataPointIndex(LocalPosition));
    return FReply::Handled();
}

void SBarChart::OnMouseLeave(const FPointerEvent& MouseEvent)
{
    ClearHover();
}
