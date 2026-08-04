#pragma once

#include "CoreMinimal.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Data/ChartStyles.h"
#include "Data/TimeSeriesData.h"
#include "Widgets/SLeafWidget.h"

DECLARE_DELEGATE_OneParam(FOnTimeSeriesSelectionChanged, int32);

class SIMPLEUECHARTS_API STimeSeriesChart : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(STimeSeriesChart) {}
        SLATE_EVENT(FOnTimeSeriesSelectionChanged, OnSelectionChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    void SetSeries(const TArray<FTimeSeries>& NewSeries);
    void ClearSeries();
    void SetChartStyle(const FChartStyle& InChartStyle);
    void SetTimeSeriesChartStyle(const FTimeSeriesChartStyle& InTimeSeriesChartStyle);
    void ClearSelection();
    int32 GetSelectedSeriesIndex() const { return SelectedSeriesIndex; }

    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

private:
    struct FCachedPoint
    {
        int32 SourcePointIndex = INDEX_NONE;
        FDateTime Timestamp;
        double Value = 0.0;
        FText LabelOverride;
        FVector2D Position = FVector2D::ZeroVector;
    };

    struct FCachedSeries
    {
        int32 SourceSeriesIndex = INDEX_NONE;
        FText Name;
        FLinearColor Color = FLinearColor::White;
        TArray<FCachedPoint> Points;
    };

    struct FCachedTick
    {
        float Position = 0.0f;
        FText Text;
        FVector2D TextPosition = FVector2D::ZeroVector;
        FVector2D TextSize = FVector2D::ZeroVector;
    };

    struct FCachedLegendEntry
    {
        int32 SourceSeriesIndex = INDEX_NONE;
        FText Text;
        FLinearColor Color = FLinearColor::White;
        FVector2D LineStart = FVector2D::ZeroVector;
        FVector2D LineEnd = FVector2D::ZeroVector;
        FVector2D TextPosition = FVector2D::ZeroVector;
        FVector2D TextSize = FVector2D::ZeroVector;
        FSlateRect HitRect;
    };

    void RecalculateChart();
    void InvalidateCachedLayout();
    void EnsureCachedLayout(const FVector2D& LocalSize) const;
    void RefreshBrushes();
    FSlateFontInfo GetChartFont() const;
    int32 HitTestSeries(const FVector2D& LocalPosition) const;
    static float DistanceToSegment(const FVector2D& Point, const FVector2D& Start, const FVector2D& End);

    TArray<FTimeSeries> Series;
    FChartStyle ChartStyle;
    FTimeSeriesChartStyle TimeSeriesChartStyle;
    int32 SelectedSeriesIndex = INDEX_NONE;
    FOnTimeSeriesSelectionChanged OnSelectionChanged;
    TUniquePtr<FSlateRoundedBoxBrush> MarkerBrush;
    TUniquePtr<FSlateRoundedBoxBrush> TooltipBrush;

    mutable bool bLayoutDirty = true;
    mutable FVector2D CachedLocalSize = FVector2D(-1.0f, -1.0f);
    mutable FVector2D CachedPlotTopLeft = FVector2D::ZeroVector;
    mutable FVector2D CachedPlotBottomRight = FVector2D::ZeroVector;
    mutable TArray<FCachedSeries> CachedSeries;
    mutable TArray<FCachedTick> CachedXTicks;
    mutable TArray<FCachedTick> CachedYTicks;
    mutable TArray<FCachedLegendEntry> CachedLegendEntries;
    int32 HoveredSeriesIndex = INDEX_NONE;
    int32 HoveredPointIndex = INDEX_NONE;
};
