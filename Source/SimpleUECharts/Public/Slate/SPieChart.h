#pragma once

#include "CoreMinimal.h"
#include "Data/ChartDataPoint.h"
#include "Data/ChartStyles.h"
#include "Rendering/RenderingCommon.h"
#include "Widgets/SLeafWidget.h"

class SIMPLEUECHARTS_API SPieChart : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SPieChart) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetData(const TArray<FChartDataPoint>& NewData);
    void ClearData();
    void SetChartStyle(const FChartStyle& InChartStyle);
    void SetPieChartStyle(const FPieChartStyle& InPieChartStyle);

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

    struct FCachedLegendEntry
    {
        FLinearColor Color = FLinearColor::White;
        FString LabelText;
        FString ValueText;
        FVector2D SwatchPosition = FVector2D::ZeroVector;
        FVector2D TextPosition = FVector2D::ZeroVector;
        FVector2D ValueTextPosition = FVector2D::ZeroVector;
    };

    void RecalculateChart();
    void ResetCalculatedState();
    void InvalidateCachedLayout();
    void EnsureCachedLayout(const FVector2D& LocalSize) const;
    FSlateFontInfo GetChartFont() const;

    TArray<FChartDataPoint> Data;
    TArray<FPieSlice> CalculatedSlices;
    float TotalValue = 0.0f;
    FChartStyle ChartStyle;
    FPieChartStyle PieChartStyle;
    mutable bool bLayoutDirty = true;
    mutable FVector2D CachedLocalSize = FVector2D(-1.0f, -1.0f);
    mutable TArray<FSlateVertex> CachedVertices;
    mutable TArray<SlateIndex> CachedIndices;
    mutable TArray<FCachedLegendEntry> CachedLegendEntries;
};
