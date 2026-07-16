#pragma once

#include "CoreMinimal.h"
#include "Data/ChartDataPoint.h"
#include "Data/ChartStyles.h"
#include "Widgets/SLeafWidget.h"

class SIMPLEUECHARTS_API SBarChart : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SBarChart) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetData(const TArray<FChartDataPoint>& NewData);
    void ClearData();
    void SetChartStyle(const FChartStyle& InChartStyle);
    void SetBarChartStyle(const FBarChartStyle& InBarChartStyle);

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
    struct FCachedBarData
    {
        FText Label;
        FText ValueText;
        float Value = 0.0f;
        float NormalizedValue = 0.0f;
        FLinearColor Color = FLinearColor::White;
    };

    struct FCachedBarLayout
    {
        FVector2D Position = FVector2D::ZeroVector;
        FVector2D Size = FVector2D::ZeroVector;
        FVector2D ValueTextPosition = FVector2D::ZeroVector;
        FVector2D LabelTextPosition = FVector2D::ZeroVector;
    };

    struct FCachedTickLayout
    {
        float Y = 0.0f;
        FText Text;
        FVector2D TextPosition = FVector2D::ZeroVector;
    };

    void RecalculateChart();
    void InvalidateCachedLayout();
    void EnsureCachedLayout(const FVector2D& LocalSize) const;
    FSlateFontInfo GetChartFont() const;

    TArray<FChartDataPoint> Data;
    TArray<FCachedBarData> CachedBars;
    FChartStyle ChartStyle;
    FBarChartStyle BarChartStyle;
    float CachedMaxValue = 0.0f;
    mutable bool bLayoutDirty = true;
    mutable FVector2D CachedLocalSize = FVector2D(-1.0f, -1.0f);
    mutable float CachedPlotLeft = 0.0f;
    mutable float CachedPlotTop = 0.0f;
    mutable float CachedPlotRight = 0.0f;
    mutable float CachedPlotBottom = 0.0f;
    mutable TArray<FCachedBarLayout> CachedBarLayouts;
    mutable TArray<FCachedTickLayout> CachedTickLayouts;
};
