#pragma once

#include "CoreMinimal.h"
#include "Data/ChartDataPoint.h"
#include "Widgets/SLeafWidget.h"

class SIMPLEUECHARTS_API SBarChart : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SBarChart) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetData(const TArray<FChartDataPoint>& NewData);
    void ClearData();
    void SetBarSpacing(float InBarSpacing);
    void SetChartPadding(const FMargin& InChartPadding);
    void SetShowLabels(bool bInShowLabels);
    void SetShowValues(bool bInShowValues);
    void SetShowYAxis(bool bInShowYAxis);
    void SetShowGridLines(bool bInShowGridLines);

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
    TArray<FChartDataPoint> Data;
    float BarSpacing = 6.0f;
    FMargin ChartPadding = FMargin(8.0f);
    bool bShowLabels = true;
    bool bShowValues = true;
    bool bShowYAxis = true;
    bool bShowGridLines = false;
};
