#include "Slate/SPieChart.h"

#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Math/UnrealMathUtility.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

namespace
{
constexpr float LegendGap = 16.0f;
constexpr float LegendSwatchSize = 10.0f;

int32 CalculateSliceSegmentCount(float SweepAngleRadians, float Radius)
{
    const float ArcLength = SweepAngleRadians * Radius;
    const int32 ByAngle = FMath::CeilToInt(SweepAngleRadians / (UE_PI / 24.0f));
    const int32 ByArcLength = FMath::CeilToInt(ArcLength / 12.0f);
    return FMath::Clamp(FMath::Max3(1, ByAngle, ByArcLength), 1, 128);
}

FString BuildLegendLabelText(const FChartDataPoint& Point, int32 FallbackIndex)
{
    if (!Point.Label.IsEmpty())
    {
        return Point.Label.ToString();
    }

    return FString::Printf(TEXT("Slice %d"), FallbackIndex + 1);
}

FString BuildLegendValueText(float Value, float Percentage, bool bIncludeValue, bool bIncludePercentage)
{
    TArray<FString> Parts;

    if (bIncludeValue)
    {
        Parts.Add(FText::AsNumber(Value).ToString());
    }

    if (bIncludePercentage)
    {
        FNumberFormattingOptions PercentFormat;
        PercentFormat.SetMaximumFractionalDigits(1);
        PercentFormat.SetMinimumFractionalDigits(0);
        Parts.Add(FString::Printf(TEXT("%s%%"), *FText::AsNumber(Percentage * 100.0f, &PercentFormat).ToString()));
    }

    return FString::Join(Parts, TEXT("  "));
}
}

void SPieChart::Construct(const FArguments& InArgs)
{
    RecalculateChart();
}

void SPieChart::SetData(const TArray<FChartDataPoint>& NewData)
{
    Data = NewData;
    RecalculateChart();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::ClearData()
{
    Data.Reset();
    ResetCalculatedState();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::SetChartStyle(const FChartStyle& InChartStyle)
{
    ChartStyle = InChartStyle;
    InvalidateCachedLayout();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::SetPieChartStyle(const FPieChartStyle& InPieChartStyle)
{
    PieChartStyle = InPieChartStyle;
    RecalculateChart();
    InvalidateCachedLayout();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::RecalculateChart()
{
    ResetCalculatedState();

    for (const FChartDataPoint& Point : Data)
    {
        if (Point.Value > 0.0f)
        {
            TotalValue += Point.Value;
        }
    }

    if (TotalValue <= 0.0f)
    {
        return;
    }

    TArray<int32> ValidIndices;
    ValidIndices.Reserve(Data.Num());

    for (int32 Index = 0; Index < Data.Num(); ++Index)
    {
        if (Data[Index].Value > 0.0f)
        {
            ValidIndices.Add(Index);
        }
    }

    if (ValidIndices.IsEmpty())
    {
        return;
    }

    constexpr float FullCircleRadians = 2.0f * UE_PI;
    const float StartAngleRadians = FMath::DegreesToRadians(PieChartStyle.StartAngle);
    const float FullCircleEndRadians = StartAngleRadians + FullCircleRadians;
    float CurrentAngleRadians = StartAngleRadians;

    CalculatedSlices.Reserve(ValidIndices.Num());

    for (int32 ValidSliceIndex = 0; ValidSliceIndex < ValidIndices.Num(); ++ValidSliceIndex)
    {
        const int32 SourceIndex = ValidIndices[ValidSliceIndex];
        const FChartDataPoint& Point = Data[SourceIndex];
        const float Percentage = Point.Value / TotalValue;
        const float RawSweepAngleRadians = Percentage * FullCircleRadians;
        const bool bIsLastSlice = ValidSliceIndex == (ValidIndices.Num() - 1);
        const float EndAngleRadians = bIsLastSlice
            ? FullCircleEndRadians
            : FMath::Min(CurrentAngleRadians + RawSweepAngleRadians, FullCircleEndRadians);

        FPieSlice& Slice = CalculatedSlices.AddDefaulted_GetRef();
        Slice.SourceIndex = SourceIndex;
        Slice.Value = Point.Value;
        Slice.Percentage = Percentage;
        Slice.StartAngleRadians = CurrentAngleRadians;
        Slice.EndAngleRadians = EndAngleRadians;
        Slice.SweepAngleRadians = FMath::Max(0.0f, EndAngleRadians - CurrentAngleRadians);
        Slice.Color = Point.Color;

        CurrentAngleRadians = EndAngleRadians;
    }

    InvalidateCachedLayout();
}

void SPieChart::ResetCalculatedState()
{
    TotalValue = 0.0f;
    CalculatedSlices.Reset();
    InvalidateCachedLayout();
}

void SPieChart::InvalidateCachedLayout()
{
    bLayoutDirty = true;
    CachedLocalSize = FVector2D(-1.0f, -1.0f);
    CachedVertices.Reset();
    CachedIndices.Reset();
    CachedLegendEntries.Reset();
}

void SPieChart::EnsureCachedLayout(const FVector2D& LocalSize) const
{
    if (!bLayoutDirty && CachedLocalSize.Equals(LocalSize, KINDA_SMALL_NUMBER))
    {
        return;
    }

    bLayoutDirty = false;
    CachedLocalSize = LocalSize;
    CachedVertices.Reset();
    CachedIndices.Reset();
    CachedLegendEntries.Reset();

    if (CalculatedSlices.IsEmpty() || LocalSize.X <= 0.0f || LocalSize.Y <= 0.0f)
    {
        return;
    }

    const FSlateFontInfo ChartFont = GetChartFont();
    const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
    const float FontHeight = FontMeasure->GetMaxCharacterHeight(ChartFont);
    const bool bShowLegendValueColumn = PieChartStyle.bShowValues || PieChartStyle.bShowPercentages;
    const float LegendWidth = PieChartStyle.bShowLegend ? FMath::Clamp(LocalSize.X * 0.42f, 160.0f, 240.0f) : 0.0f;
    const float HorizontalPadding = ChartStyle.Padding.Left + ChartStyle.Padding.Right;
    const float VerticalPadding = ChartStyle.Padding.Top + ChartStyle.Padding.Bottom;
    const float PieAreaWidth = LocalSize.X - HorizontalPadding - LegendWidth - (PieChartStyle.bShowLegend ? LegendGap : 0.0f);
    const float PieAreaHeight = LocalSize.Y - VerticalPadding;
    const float Radius = 0.5f * FMath::Min(PieAreaWidth, PieAreaHeight);

    if (PieAreaWidth <= 0.0f || PieAreaHeight <= 0.0f || Radius <= KINDA_SMALL_NUMBER)
    {
        return;
    }

    const FVector2f Center(
        ChartStyle.Padding.Left + (PieAreaWidth * 0.5f),
        ChartStyle.Padding.Top + (PieAreaHeight * 0.5f));
    const float InnerRadiusPixels = PieChartStyle.InnerRadius * Radius;

    const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush(TEXT("GenericWhiteBox"));
    if (WhiteBrush == nullptr)
    {
        return;
    }

    const FSlateResourceHandle ResourceHandle = WhiteBrush->GetRenderingResource();
    if (!ResourceHandle.IsValid())
    {
        return;
    }

    const FSlateShaderResourceProxy* ResourceProxy = ResourceHandle.GetResourceProxy();
    FVector2f UVStart = FVector2f::ZeroVector;
    FVector2f UVSize = FVector2f(1.0f, 1.0f);
    if (ResourceProxy != nullptr)
    {
        UVStart = ResourceProxy->StartUV;
        UVSize = ResourceProxy->SizeUV;
    }

    const FSlateRenderTransform RenderTransform = FSlateRenderTransform();
    const float LegendEntryHeight = FMath::Max(
        FontHeight + ChartStyle.LabelPadding.Top + ChartStyle.LabelPadding.Bottom,
        LegendSwatchSize + ChartStyle.LabelPadding.Top + ChartStyle.LabelPadding.Bottom);

    int32 EstimatedVertexCount = 0;
    int32 EstimatedIndexCount = 0;
    for (const FPieSlice& Slice : CalculatedSlices)
    {
        const float GapAngle = FMath::Min(PieChartStyle.SliceSpacing / FMath::Max(Radius, 1.0f), Slice.SweepAngleRadians * 0.9f);
        const float RenderSweep = FMath::Max(0.0f, Slice.SweepAngleRadians - GapAngle);
        const int32 SegmentCount = CalculateSliceSegmentCount(RenderSweep, Radius);
        EstimatedVertexCount += InnerRadiusPixels > KINDA_SMALL_NUMBER ? (SegmentCount + 1) * 2 : SegmentCount + 2;
        EstimatedIndexCount += InnerRadiusPixels > KINDA_SMALL_NUMBER ? SegmentCount * 6 : SegmentCount * 3;
    }

    CachedVertices.Reserve(EstimatedVertexCount);
    CachedIndices.Reserve(EstimatedIndexCount);

    for (const FPieSlice& Slice : CalculatedSlices)
    {
        if (Slice.SweepAngleRadians <= KINDA_SMALL_NUMBER)
        {
            continue;
        }

        const float GapAngle = FMath::Min(PieChartStyle.SliceSpacing / FMath::Max(Radius, 1.0f), Slice.SweepAngleRadians * 0.9f);
        const float RenderStartAngle = Slice.StartAngleRadians + (GapAngle * 0.5f);
        const float RenderEndAngle = Slice.EndAngleRadians - (GapAngle * 0.5f);
        const float RenderSweep = RenderEndAngle - RenderStartAngle;
        if (RenderSweep <= KINDA_SMALL_NUMBER)
        {
            continue;
        }

        const int32 SegmentCount = CalculateSliceSegmentCount(RenderSweep, Radius);
        const FColor SliceColor = Slice.Color.ToFColor(true);

        if (InnerRadiusPixels > KINDA_SMALL_NUMBER)
        {
            const SlateIndex StartIndex = static_cast<SlateIndex>(CachedVertices.Num());

            for (int32 SegmentIndex = 0; SegmentIndex <= SegmentCount; ++SegmentIndex)
            {
                const float Alpha = static_cast<float>(SegmentIndex) / SegmentCount;
                const float AngleRadians = SegmentIndex == SegmentCount
                    ? RenderEndAngle
                    : FMath::Lerp(RenderStartAngle, RenderEndAngle, Alpha);

                float SinAngle = 0.0f;
                float CosAngle = 0.0f;
                FMath::SinCos(&SinAngle, &CosAngle, AngleRadians);

                const FVector2f UnitDirection(CosAngle, SinAngle);
                const FVector2f OuterPosition = Center + (UnitDirection * Radius);
                const FVector2f InnerPosition = Center + (UnitDirection * InnerRadiusPixels);
                const FVector2f OuterTexCoord = FVector2f(0.5f) + (UnitDirection * 0.5f);
                const FVector2f InnerTexCoord = FVector2f(0.5f) + (UnitDirection * (0.5f * PieChartStyle.InnerRadius));

                CachedVertices.Add(FSlateVertex::Make(
                    RenderTransform,
                    OuterPosition,
                    FVector2f(UVStart.X + (UVSize.X * OuterTexCoord.X), UVStart.Y + (UVSize.Y * OuterTexCoord.Y)),
                    SliceColor));
                CachedVertices.Add(FSlateVertex::Make(
                    RenderTransform,
                    InnerPosition,
                    FVector2f(UVStart.X + (UVSize.X * InnerTexCoord.X), UVStart.Y + (UVSize.Y * InnerTexCoord.Y)),
                    SliceColor));
            }

            for (int32 SegmentIndex = 0; SegmentIndex < SegmentCount; ++SegmentIndex)
            {
                const SlateIndex OuterCurrent = StartIndex + (SegmentIndex * 2);
                const SlateIndex InnerCurrent = OuterCurrent + 1;
                const SlateIndex OuterNext = OuterCurrent + 2;
                const SlateIndex InnerNext = OuterCurrent + 3;

                CachedIndices.Add(OuterCurrent);
                CachedIndices.Add(OuterNext);
                CachedIndices.Add(InnerCurrent);

                CachedIndices.Add(InnerCurrent);
                CachedIndices.Add(OuterNext);
                CachedIndices.Add(InnerNext);
            }
        }
        else
        {
            const SlateIndex CenterIndex = static_cast<SlateIndex>(CachedVertices.Num());
            CachedVertices.Add(FSlateVertex::Make(
                RenderTransform,
                Center,
                FVector2f(UVStart.X + (UVSize.X * 0.5f), UVStart.Y + (UVSize.Y * 0.5f)),
                SliceColor));

            for (int32 SegmentIndex = 0; SegmentIndex <= SegmentCount; ++SegmentIndex)
            {
                const float Alpha = static_cast<float>(SegmentIndex) / SegmentCount;
                const float AngleRadians = SegmentIndex == SegmentCount
                    ? RenderEndAngle
                    : FMath::Lerp(RenderStartAngle, RenderEndAngle, Alpha);

                float SinAngle = 0.0f;
                float CosAngle = 0.0f;
                FMath::SinCos(&SinAngle, &CosAngle, AngleRadians);

                const FVector2f UnitDirection(CosAngle, SinAngle);
                const FVector2f Position = Center + (UnitDirection * Radius);
                const FVector2f TexCoord = FVector2f(0.5f) + (UnitDirection * 0.5f);

                CachedVertices.Add(FSlateVertex::Make(
                    RenderTransform,
                    Position,
                    FVector2f(UVStart.X + (UVSize.X * TexCoord.X), UVStart.Y + (UVSize.Y * TexCoord.Y)),
                    SliceColor));
            }

            for (int32 SegmentIndex = 0; SegmentIndex < SegmentCount; ++SegmentIndex)
            {
                CachedIndices.Add(CenterIndex);
                CachedIndices.Add(CenterIndex + SegmentIndex + 1);
                CachedIndices.Add(CenterIndex + SegmentIndex + 2);
            }
        }
    }

    if (PieChartStyle.bShowLegend)
    {
        const float LegendX = ChartStyle.Padding.Left + PieAreaWidth + LegendGap;
        const float TotalLegendHeight = CalculatedSlices.Num() * LegendEntryHeight;
        float LegendY = ChartStyle.Padding.Top + FMath::Max((PieAreaHeight - TotalLegendHeight) * 0.5f, 0.0f);

        for (int32 SliceIndex = 0; SliceIndex < CalculatedSlices.Num(); ++SliceIndex)
        {
            const FPieSlice& Slice = CalculatedSlices[SliceIndex];
            const FChartDataPoint& Point = Data[Slice.SourceIndex];

            FCachedLegendEntry& LegendEntry = CachedLegendEntries.AddDefaulted_GetRef();
            LegendEntry.Color = Point.Color;
            LegendEntry.LabelText = PieChartStyle.bShowLabels ? BuildLegendLabelText(Point, SliceIndex) : FString();
            LegendEntry.ValueText = BuildLegendValueText(
                Slice.Value,
                Slice.Percentage,
                PieChartStyle.bShowValues,
                PieChartStyle.bShowPercentages);
            LegendEntry.SwatchPosition = FVector2D(
                LegendX,
                LegendY + ChartStyle.LabelPadding.Top + FMath::Max((FontHeight - LegendSwatchSize) * 0.5f, 0.0f));
            LegendEntry.TextPosition = FVector2D(
                LegendX + LegendSwatchSize + ChartStyle.LabelPadding.Left,
                LegendY + ChartStyle.LabelPadding.Top);

            if (!LegendEntry.ValueText.IsEmpty() && bShowLegendValueColumn)
            {
                const FVector2D ValueTextSize = FontMeasure->Measure(LegendEntry.ValueText, ChartFont);
                LegendEntry.ValueTextPosition = FVector2D(
                    LegendX + LegendWidth - ValueTextSize.X,
                    LegendY + ChartStyle.LabelPadding.Top);
            }
            else
            {
                LegendEntry.ValueTextPosition = FVector2D::ZeroVector;
            }

            LegendY += LegendEntryHeight;
        }
    }
}

FSlateFontInfo SPieChart::GetChartFont() const
{
    return ChartStyle.Font.Size > 0
        ? ChartStyle.Font
        : FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 10);
}

FVector2D SPieChart::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return FVector2D(320.0f, 220.0f);
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
    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush(TEXT("GenericWhiteBox"));
    const FSlateFontInfo ChartFont = GetChartFont();
    const FSlateResourceHandle ResourceHandle = WhiteBrush->GetRenderingResource();
    const ESlateDrawEffect DrawEffects = bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
    const FLinearColor Tint = InWidgetStyle.GetColorAndOpacityTint();
    const FLinearColor TextColor = ChartStyle.TextColor * Tint;
    const FLinearColor BackgroundColor = ChartStyle.BackgroundColor * Tint;
    const FSlateRenderTransform& RenderTransform = AllottedGeometry.GetAccumulatedRenderTransform();
    const bool bHasValidBackground = ChartStyle.BackgroundColor.A > 0.0f;

    if (bHasValidBackground)
    {
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(LocalSize, FSlateLayoutTransform()),
            WhiteBrush,
            DrawEffects,
            BackgroundColor);
    }

    if (CalculatedSlices.IsEmpty())
    {
        return bHasValidBackground ? LayerId : LayerId;
    }

    EnsureCachedLayout(LocalSize);

    if (CachedVertices.IsEmpty() || CachedIndices.IsEmpty())
    {
        return bHasValidBackground ? LayerId : LayerId;
    }

    TArray<FSlateVertex> TransformedVertices;
    TransformedVertices.Reserve(CachedVertices.Num());

    for (const FSlateVertex& Vertex : CachedVertices)
    {
        const FLinearColor VertexColor = Vertex.Color.ReinterpretAsLinear() * Tint;
        TransformedVertices.Add(FSlateVertex::Make(
            RenderTransform,
            Vertex.Position,
            FVector2f(Vertex.TexCoords[0], Vertex.TexCoords[1]),
            VertexColor.ToFColor(true)));
    }

    FSlateDrawElement::MakeCustomVerts(
        OutDrawElements,
        LayerId + (bHasValidBackground ? 1 : 0),
        ResourceHandle,
        TransformedVertices,
        CachedIndices,
        nullptr,
        0,
        0,
        DrawEffects);

    if (PieChartStyle.bShowLegend)
    {
        for (const FCachedLegendEntry& LegendEntry : CachedLegendEntries)
        {
            const FVector2D SwatchSize(LegendSwatchSize, LegendSwatchSize);

            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId + (bHasValidBackground ? 2 : 1),
                AllottedGeometry.ToPaintGeometry(SwatchSize, FSlateLayoutTransform(LegendEntry.SwatchPosition)),
                WhiteBrush,
                DrawEffects,
                LegendEntry.Color * Tint);

            if (!LegendEntry.LabelText.IsEmpty())
            {
                FSlateDrawElement::MakeText(
                    OutDrawElements,
                    LayerId + (bHasValidBackground ? 2 : 1),
                    AllottedGeometry.ToOffsetPaintGeometry(LegendEntry.TextPosition),
                    FText::FromString(LegendEntry.LabelText),
                    ChartFont,
                    DrawEffects,
                    TextColor);
            }

            if (!LegendEntry.ValueText.IsEmpty())
            {
                FSlateDrawElement::MakeText(
                    OutDrawElements,
                    LayerId + (bHasValidBackground ? 2 : 1),
                    AllottedGeometry.ToOffsetPaintGeometry(LegendEntry.ValueTextPosition),
                    FText::FromString(LegendEntry.ValueText),
                    ChartFont,
                    DrawEffects,
                    TextColor);
            }
        }
    }

    return LayerId + (bHasValidBackground ? 2 : 1);
}
