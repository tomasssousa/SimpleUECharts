#pragma once

#include "CoreMinimal.h"
#include "Data/ChartDataPoint.h"
#include "Data/ChartHover.h"
#include "Data/ChartStyles.h"
#include "Rendering/RenderingCommon.h"
#include "Widgets/SLeafWidget.h"

class SIMPLEUECHARTS_API SPieChart : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SPieChart) {}
        SLATE_EVENT(FOnSlateChartHoverChanged, OnHoveredDataPointChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetData(const TArray<FChartDataPoint>& NewData);
    void ClearData();
    void SetChartStyle(const FChartStyle& InChartStyle);
    void SetPieChartStyle(const FPieChartStyle& InPieChartStyle);
    int32 GetHoveredDataPointIndex() const;

    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

    virtual int32 OnPaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled) const override;

    virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

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

    struct FCachedSliceHitTest
    {
        int32 SourceIndex = INDEX_NONE;
        float StartAngleRadians = 0.0f;
        float EndAngleRadians = 0.0f;
    };

    void RecalculateChart();
    void ResetCalculatedState();
    void InvalidateCachedLayout();
    void EnsureCachedLayout(const FVector2D& LocalSize) const;
    FSlateFontInfo GetChartFont() const;
    int32 FindHoveredDataPointIndex(const FVector2D& LocalPosition) const;
    void SetHoveredDataPointIndex(int32 NewHoveredDataPointIndex, const FVector2D& LocalPosition);
    void ClearHover();

    TArray<FChartDataPoint> Data;
    TArray<FPieSlice> CalculatedSlices;
    float TotalValue = 0.0f;
    FChartStyle ChartStyle;
    FPieChartStyle PieChartStyle;
    int32 HoveredDataPointIndex = INDEX_NONE;
    FOnSlateChartHoverChanged OnHoveredDataPointChanged;
    mutable bool bLayoutDirty = true;
    mutable FVector2D CachedLocalSize = FVector2D(-1.0f, -1.0f);
    mutable FVector2D CachedPieCenter = FVector2D::ZeroVector;
    mutable float CachedOuterRadius = 0.0f;
    mutable float CachedInnerRadius = 0.0f;
    mutable TArray<FSlateVertex> CachedVertices;
    mutable TArray<SlateIndex> CachedIndices;
    mutable TArray<int32> CachedVertexDataPointIndices;
    mutable TArray<FCachedSliceHitTest> CachedSliceHitTests;
    mutable TArray<FCachedLegendEntry> CachedLegendEntries;
};
