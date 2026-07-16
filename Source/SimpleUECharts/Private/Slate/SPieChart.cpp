#include "Slate/SPieChart.h"

#include "Math/UnrealMathUtility.h"

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
    // Phase 8 prepares validated slice angles; phase 9 will render them.
    return LayerId;
}
