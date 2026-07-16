#pragma once

#include "CoreMinimal.h"
#include "Data/ChartDataPoint.h"
#include "Widgets/SLeafWidget.h"

class SIMPLEUECHARTS_API SPieChart : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SPieChart) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetData(const TArray<FChartDataPoint>& NewData);
    void ClearData();
    void SetShowLabels(bool bInShowLabels);
    void SetShowValues(bool bInShowValues);
    void SetShowPercentages(bool bInShowPercentages);
    void SetShowLegend(bool bInShowLegend);
    void SetStartAngle(float InStartAngleDegrees);
    void SetSliceSpacing(float InSliceSpacing);
    void SetInnerRadius(float InInnerRadius);

    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

    virtual int32 OnPaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled) const override;

private:
    struct FPieSlice
    {
        int32 SourceIndex = INDEX_NONE;
        float Value = 0.0f;
        float Percentage = 0.0f;
        float StartAngleRadians = 0.0f;
        float EndAngleRadians = 0.0f;
        float SweepAngleRadians = 0.0f;
        FLinearColor Color = FLinearColor::White;
    };

    void RecalculateChart();
    void ResetCalculatedState();

    TArray<FChartDataPoint> Data;
    TArray<FPieSlice> CalculatedSlices;
    float TotalValue = 0.0f;
    float StartAngleDegrees = 0.0f;
    float SliceSpacing = 0.0f;
    float InnerRadius = 0.0f;
    bool bShowLabels = true;
    bool bShowValues = true;
    bool bShowPercentages = true;
    bool bShowLegend = true;
};
