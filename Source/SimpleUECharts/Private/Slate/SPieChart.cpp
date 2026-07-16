#include "Slate/SPieChart.h"

#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Math/UnrealMathUtility.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

namespace
{
constexpr float ChartPadding = 12.0f;
constexpr float LegendGap = 16.0f;
constexpr float LegendSwatchSize = 10.0f;
constexpr float LegendEntryHeight = 20.0f;

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

void SPieChart::SetShowLabels(bool bInShowLabels)
{
    bShowLabels = bInShowLabels;
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::SetShowValues(bool bInShowValues)
{
    bShowValues = bInShowValues;
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::SetShowPercentages(bool bInShowPercentages)
{
    bShowPercentages = bInShowPercentages;
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::SetShowLegend(bool bInShowLegend)
{
    bShowLegend = bInShowLegend;
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::SetStartAngle(float InStartAngleDegrees)
{
    StartAngleDegrees = InStartAngleDegrees;
    RecalculateChart();
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::SetSliceSpacing(float InSliceSpacing)
{
    SliceSpacing = FMath::Max(0.0f, InSliceSpacing);
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SPieChart::SetInnerRadius(float InInnerRadius)
{
    InnerRadius = FMath::Clamp(InInnerRadius, 0.0f, 0.95f);
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
    const float StartAngleRadians = FMath::DegreesToRadians(StartAngleDegrees);
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
}

void SPieChart::ResetCalculatedState()
{
    TotalValue = 0.0f;
    CalculatedSlices.Reset();
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
    if (CalculatedSlices.IsEmpty())
    {
        return LayerId;
    }

    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    if (LocalSize.X <= 0.0f || LocalSize.Y <= 0.0f)
    {
        return LayerId;
    }

    const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush(TEXT("GenericWhiteBox"));
    if (WhiteBrush == nullptr)
    {
        return LayerId;
    }

    const FSlateResourceHandle ResourceHandle = WhiteBrush->GetRenderingResource();
    if (!ResourceHandle.IsValid())
    {
        return LayerId;
    }

    const FSlateShaderResourceProxy* ResourceProxy = ResourceHandle.GetResourceProxy();
    FVector2f UVStart = FVector2f::ZeroVector;
    FVector2f UVSize = FVector2f(1.0f, 1.0f);
    if (ResourceProxy != nullptr)
    {
        UVStart = ResourceProxy->StartUV;
        UVSize = ResourceProxy->SizeUV;
    }

    static const FSlateFontInfo ChartFont = FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 10);
    const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
    const FSlateRenderTransform& RenderTransform = AllottedGeometry.GetAccumulatedRenderTransform();
    const ESlateDrawEffect DrawEffects = bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
    const FLinearColor Tint = InWidgetStyle.GetColorAndOpacityTint();
    const FLinearColor TextColor = FLinearColor::White * Tint;
    const bool bShowLegendValueColumn = bShowValues || bShowPercentages;
    const float LegendWidth = bShowLegend ? FMath::Clamp(LocalSize.X * 0.42f, 160.0f, 240.0f) : 0.0f;
    const float PieAreaWidth = LocalSize.X - (ChartPadding * 2.0f) - LegendWidth - (bShowLegend ? LegendGap : 0.0f);
    const float PieAreaHeight = LocalSize.Y - (ChartPadding * 2.0f);
    const float Radius = 0.5f * FMath::Min(PieAreaWidth, PieAreaHeight);

    if (PieAreaWidth <= 0.0f || PieAreaHeight <= 0.0f || Radius <= KINDA_SMALL_NUMBER)
    {
        return LayerId;
    }

    const FVector2f Center(
        ChartPadding + (PieAreaWidth * 0.5f),
        ChartPadding + (PieAreaHeight * 0.5f));
    const float InnerRadiusPixels = InnerRadius * Radius;

    TArray<FSlateVertex> Vertices;
    TArray<SlateIndex> Indices;

    int32 EstimatedVertexCount = 0;
    int32 EstimatedIndexCount = 0;
    for (const FPieSlice& Slice : CalculatedSlices)
    {
        const float GapAngle = FMath::Min(SliceSpacing / FMath::Max(Radius, 1.0f), Slice.SweepAngleRadians * 0.9f);
        const float RenderSweep = FMath::Max(0.0f, Slice.SweepAngleRadians - GapAngle);
        const int32 SegmentCount = CalculateSliceSegmentCount(RenderSweep, Radius);
        EstimatedVertexCount += InnerRadiusPixels > KINDA_SMALL_NUMBER ? (SegmentCount + 1) * 2 : SegmentCount + 2;
        EstimatedIndexCount += InnerRadiusPixels > KINDA_SMALL_NUMBER ? SegmentCount * 6 : SegmentCount * 3;
    }

    Vertices.Reserve(EstimatedVertexCount);
    Indices.Reserve(EstimatedIndexCount);

    for (const FPieSlice& Slice : CalculatedSlices)
    {
        if (Slice.SweepAngleRadians <= KINDA_SMALL_NUMBER)
        {
            continue;
        }

        const float GapAngle = FMath::Min(SliceSpacing / FMath::Max(Radius, 1.0f), Slice.SweepAngleRadians * 0.9f);
        const float RenderStartAngle = Slice.StartAngleRadians + (GapAngle * 0.5f);
        const float RenderEndAngle = Slice.EndAngleRadians - (GapAngle * 0.5f);
        const float RenderSweep = RenderEndAngle - RenderStartAngle;
        if (RenderSweep <= KINDA_SMALL_NUMBER)
        {
            continue;
        }

        const int32 SegmentCount = CalculateSliceSegmentCount(RenderSweep, Radius);
        const FColor SliceColor = (Slice.Color * Tint).ToFColor(true);

        if (InnerRadiusPixels > KINDA_SMALL_NUMBER)
        {
            const SlateIndex StartIndex = static_cast<SlateIndex>(Vertices.Num());

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
                const FVector2f InnerTexCoord = FVector2f(0.5f) + (UnitDirection * (0.5f * InnerRadius));

                Vertices.Add(FSlateVertex::Make(
                    RenderTransform,
                    OuterPosition,
                    FVector2f(UVStart.X + (UVSize.X * OuterTexCoord.X), UVStart.Y + (UVSize.Y * OuterTexCoord.Y)),
                    SliceColor));
                Vertices.Add(FSlateVertex::Make(
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

                Indices.Add(OuterCurrent);
                Indices.Add(OuterNext);
                Indices.Add(InnerCurrent);

                Indices.Add(InnerCurrent);
                Indices.Add(OuterNext);
                Indices.Add(InnerNext);
            }
        }
        else
        {
            const SlateIndex CenterIndex = static_cast<SlateIndex>(Vertices.Num());
            Vertices.Add(FSlateVertex::Make(
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

                Vertices.Add(FSlateVertex::Make(
                    RenderTransform,
                    Position,
                    FVector2f(UVStart.X + (UVSize.X * TexCoord.X), UVStart.Y + (UVSize.Y * TexCoord.Y)),
                    SliceColor));
            }

            for (int32 SegmentIndex = 0; SegmentIndex < SegmentCount; ++SegmentIndex)
            {
                Indices.Add(CenterIndex);
                Indices.Add(CenterIndex + SegmentIndex + 1);
                Indices.Add(CenterIndex + SegmentIndex + 2);
            }
        }

    }

    if (!Vertices.IsEmpty() && !Indices.IsEmpty())
    {
        FSlateDrawElement::MakeCustomVerts(
            OutDrawElements,
            LayerId,
            ResourceHandle,
            Vertices,
            Indices,
            nullptr,
            0,
            0,
            DrawEffects);
    }

    if (bShowLegend)
    {
        const float LegendX = ChartPadding + PieAreaWidth + LegendGap;
        const float TotalLegendHeight = CalculatedSlices.Num() * LegendEntryHeight;
        float LegendY = ChartPadding + FMath::Max((PieAreaHeight - TotalLegendHeight) * 0.5f, 0.0f);
        float MaxValueTextWidth = 0.0f;

        if (bShowLegendValueColumn)
        {
            for (const FPieSlice& Slice : CalculatedSlices)
            {
                const FString ValueText = BuildLegendValueText(Slice.Value, Slice.Percentage, bShowValues, bShowPercentages);
                MaxValueTextWidth = FMath::Max(MaxValueTextWidth, FontMeasure->Measure(ValueText, ChartFont).X);
            }
        }

        for (int32 SliceIndex = 0; SliceIndex < CalculatedSlices.Num(); ++SliceIndex)
        {
            const FPieSlice& Slice = CalculatedSlices[SliceIndex];
            const FChartDataPoint& Point = Data[Slice.SourceIndex];
            const FString LabelText = bShowLabels ? BuildLegendLabelText(Point, SliceIndex) : FString();
            const FString ValueText = BuildLegendValueText(Slice.Value, Slice.Percentage, bShowValues, bShowPercentages);
            const FVector2D SwatchPosition(LegendX, LegendY + 4.0f);
            const FVector2D SwatchSize(LegendSwatchSize, LegendSwatchSize);

            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId + 1,
                AllottedGeometry.ToPaintGeometry(SwatchSize, FSlateLayoutTransform(SwatchPosition)),
                WhiteBrush,
                DrawEffects,
                Point.Color * Tint);

            if (!LabelText.IsEmpty())
            {
                FSlateDrawElement::MakeText(
                    OutDrawElements,
                    LayerId + 1,
                    AllottedGeometry.ToOffsetPaintGeometry(FVector2D(LegendX + LegendSwatchSize + 12.0f, LegendY)),
                    FText::FromString(LabelText),
                    ChartFont,
                    DrawEffects,
                    TextColor);
            }

            if (!ValueText.IsEmpty())
            {
                const FVector2D ValueTextSize = FontMeasure->Measure(ValueText, ChartFont);
                const float ValueX = LegendX + LegendWidth - ValueTextSize.X;

                FSlateDrawElement::MakeText(
                    OutDrawElements,
                    LayerId + 1,
                    AllottedGeometry.ToOffsetPaintGeometry(FVector2D(ValueX, LegendY)),
                    FText::FromString(ValueText),
                    ChartFont,
                    DrawEffects,
                    TextColor);
            }

            LegendY += LegendEntryHeight;
        }
    }

    return LayerId + 1;
}
