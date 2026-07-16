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

void SBarChart::SetChartStyle(const FChartStyle& InChartStyle)
{
    ChartStyle = InChartStyle;
    RecalculateChart();
    InvalidateCachedLayout();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SBarChart::SetBarChartStyle(const FBarChartStyle& InBarChartStyle)
{
    BarChartStyle = InBarChartStyle;
    RecalculateChart();
    InvalidateCachedLayout();
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

    const FSlateFontInfo ChartFont = GetChartFont();
    const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
    const float FontHeight = FontMeasure->GetMaxCharacterHeight(ChartFont);
    const float LabelSpacing = ChartStyle.LabelPadding.Top + ChartStyle.LabelPadding.Bottom;
    const float ValueSpacing = ChartStyle.LabelPadding.Top + ChartStyle.LabelPadding.Bottom;
    const float BottomLabelHeight = BarChartStyle.bShowLabels ? FontHeight + LabelSpacing : 0.0f;
    const float TopValueHeight = BarChartStyle.bShowValues ? FontHeight + ValueSpacing : 0.0f;
    const float AxisLabelWidth = BarChartStyle.bShowYAxis ? 32.0f : 0.0f;
    const float PlotLeft = ChartStyle.Padding.Left + AxisLabelWidth;
    const float PlotTop = ChartStyle.Padding.Top + TopValueHeight;
    const float PlotRight = LocalSize.X - ChartStyle.Padding.Right;
    const float PlotBottom = LocalSize.Y - ChartStyle.Padding.Bottom - BottomLabelHeight;
    const float PlotWidth = PlotRight - PlotLeft;
    const float PlotHeight = PlotBottom - PlotTop;
    const float AvailableWidth = PlotWidth - (BarChartStyle.BarSpacing * (CachedBars.Num() - 1));
    const float AvailableHeight = PlotHeight;

    if (PlotWidth <= 0.0f || PlotHeight <= 0.0f || AvailableWidth <= 0.0f || AvailableHeight <= 0.0f)
    {
        return;
    }

    const float BaseBarWidth = AvailableWidth / CachedBars.Num();
    const float DesiredMinimumBarWidth = FMath::Max(0.0f, BarChartStyle.MinimumBarWidth);
    const float DesiredMaximumBarWidth = BarChartStyle.MaximumBarWidth > 0.0f
        ? FMath::Max(DesiredMinimumBarWidth, BarChartStyle.MaximumBarWidth)
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
            (BarChartStyle.BarSpacing * (CachedBars.Num() - 1));

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
        (BarChartStyle.BarSpacing * (CachedBars.Num() - 1));
    const float BarStartX = PlotLeft + FMath::Max((PlotWidth - OccupiedWidth) * 0.5f, 0.0f);

    CachedPlotLeft = PlotLeft;
    CachedPlotTop = PlotTop;
    CachedPlotRight = PlotRight;
    CachedPlotBottom = PlotBottom;

    const int32 TickCount = 4;
    CachedTickLayouts.Reserve(TickCount);
    for (int32 TickIndex = 1; TickIndex <= TickCount; ++TickIndex)
    {
        const float TickAlpha = static_cast<float>(TickIndex) / TickCount;
        FCachedTickLayout& TickLayout = CachedTickLayouts.AddDefaulted_GetRef();
        TickLayout.Y = PlotBottom - (TickAlpha * PlotHeight);
        TickLayout.Text = FText::AsNumber(CachedMaxValue * TickAlpha);
        TickLayout.TextPosition = FVector2D(
            ChartStyle.Padding.Left,
            TickLayout.Y - (FontHeight * 0.5f));
    }

    CachedBarLayouts.Reserve(CachedBars.Num());
    for (int32 Index = 0; Index < CachedBars.Num(); ++Index)
    {
        const FCachedBarData& CachedBar = CachedBars[Index];
        const float BarHeight = CachedBar.NormalizedValue * AvailableHeight;
        const float BarX = BarStartX + (Index * (BarWidth + BarChartStyle.BarSpacing));
        const float BarY = PlotBottom - BarHeight;

        FCachedBarLayout& BarLayout = CachedBarLayouts.AddDefaulted_GetRef();
        BarLayout.Position = FVector2D(BarX, BarY);
        BarLayout.Size = FVector2D(BarWidth, BarHeight);
        const FVector2D ValueTextSize = FontMeasure->Measure(CachedBar.ValueText, ChartFont);
        const float ValueX = BarX + FMath::Max((BarWidth - ValueTextSize.X) * 0.5f, 0.0f);
        const float ValueY = FMath::Max(
            ChartStyle.Padding.Top,
            BarY - ValueTextSize.Y - ChartStyle.LabelPadding.Bottom);
        BarLayout.ValueTextPosition = FVector2D(ValueX, ValueY);

        const FVector2D LabelTextSize = FontMeasure->Measure(CachedBar.Label, ChartFont);
        const float LabelX = BarX + FMath::Max((BarWidth - LabelTextSize.X) * 0.5f, 0.0f);
        BarLayout.LabelTextPosition = FVector2D(LabelX, PlotBottom + ChartStyle.LabelPadding.Top);
    }
}

FSlateFontInfo SBarChart::GetChartFont() const
{
    return ChartStyle.Font.Size > 0
        ? ChartStyle.Font
        : FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 10);
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
    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
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
            1.0f);
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
                1.0f);
        }

        if (BarChartStyle.bShowYAxis)
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
