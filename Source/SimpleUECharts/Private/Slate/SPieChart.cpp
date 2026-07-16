#include "Slate/SPieChart.h"

#include "Math/UnrealMathUtility.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

namespace
{
int32 CalculateSliceSegmentCount(float SweepAngleRadians, float Radius)
{
    const float ArcLength = SweepAngleRadians * Radius;
    const int32 ByAngle = FMath::CeilToInt(SweepAngleRadians / (UE_PI / 24.0f));
    const int32 ByArcLength = FMath::CeilToInt(ArcLength / 12.0f);
    return FMath::Clamp(FMath::Max3(1, ByAngle, ByArcLength), 1, 128);
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
    float CurrentAngleRadians = 0.0f;

    CalculatedSlices.Reserve(ValidIndices.Num());

    for (int32 ValidSliceIndex = 0; ValidSliceIndex < ValidIndices.Num(); ++ValidSliceIndex)
    {
        const int32 SourceIndex = ValidIndices[ValidSliceIndex];
        const FChartDataPoint& Point = Data[SourceIndex];
        const float Percentage = Point.Value / TotalValue;
        const float RawSweepAngleRadians = Percentage * FullCircleRadians;
        const bool bIsLastSlice = ValidSliceIndex == (ValidIndices.Num() - 1);
        const float EndAngleRadians = bIsLastSlice
            ? FullCircleRadians
            : FMath::Min(CurrentAngleRadians + RawSweepAngleRadians, FullCircleRadians);

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
    if (CalculatedSlices.IsEmpty())
    {
        return LayerId;
    }

    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const float Radius = 0.5f * FMath::Min(LocalSize.X, LocalSize.Y);

    if (Radius <= KINDA_SMALL_NUMBER)
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

    const FVector2f Center = FVector2f(LocalSize * 0.5f);
    const FSlateRenderTransform& RenderTransform = AllottedGeometry.GetAccumulatedRenderTransform();
    const ESlateDrawEffect DrawEffects = bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
    const FLinearColor Tint = InWidgetStyle.GetColorAndOpacityTint();

    TArray<FSlateVertex> Vertices;
    TArray<SlateIndex> Indices;

    int32 EstimatedVertexCount = 0;
    int32 EstimatedIndexCount = 0;
    for (const FPieSlice& Slice : CalculatedSlices)
    {
        const int32 SegmentCount = CalculateSliceSegmentCount(Slice.SweepAngleRadians, Radius);
        EstimatedVertexCount += SegmentCount + 2;
        EstimatedIndexCount += SegmentCount * 3;
    }

    Vertices.Reserve(EstimatedVertexCount);
    Indices.Reserve(EstimatedIndexCount);

    for (const FPieSlice& Slice : CalculatedSlices)
    {
        if (Slice.SweepAngleRadians <= KINDA_SMALL_NUMBER)
        {
            continue;
        }

        const int32 SegmentCount = CalculateSliceSegmentCount(Slice.SweepAngleRadians, Radius);
        const SlateIndex CenterIndex = static_cast<SlateIndex>(Vertices.Num());
        const FColor SliceColor = (Slice.Color * Tint).ToFColor(true);

        Vertices.Add(FSlateVertex::Make(
            RenderTransform,
            Center,
            FVector2f(UVStart.X + (UVSize.X * 0.5f), UVStart.Y + (UVSize.Y * 0.5f)),
            SliceColor));

        for (int32 SegmentIndex = 0; SegmentIndex <= SegmentCount; ++SegmentIndex)
        {
            const float Alpha = static_cast<float>(SegmentIndex) / SegmentCount;
            const float AngleRadians = SegmentIndex == SegmentCount
                ? Slice.EndAngleRadians
                : FMath::Lerp(Slice.StartAngleRadians, Slice.EndAngleRadians, Alpha);

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

    return LayerId;
}
