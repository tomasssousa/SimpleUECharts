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
};
