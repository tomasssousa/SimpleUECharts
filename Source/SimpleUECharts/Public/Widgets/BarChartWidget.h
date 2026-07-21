#pragma once

#include "Components/Widget.h"
#include "Data/ChartDataPoint.h"
#include "Data/ChartHover.h"
#include "Data/ChartStyles.h"
#include "BarChartWidget.generated.h"

class SBarChart;

UCLASS(meta = (DisplayName = "Bar Chart"))
class SIMPLEUECHARTS_API UBarChartWidget : public UWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    TArray<FChartDataPoint> Data;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    FChartStyle ChartStyle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    FBarChartStyle BarChartStyle;

    UFUNCTION(BlueprintCallable, Category = "Chart")
    void SetData(const TArray<FChartDataPoint>& NewData);

    UFUNCTION(BlueprintCallable, Category = "Chart")
    void ClearData();

    UFUNCTION(BlueprintCallable, Category = "Style")
    void SetChartStyle(const FChartStyle& NewChartStyle);

    UFUNCTION(BlueprintCallable, Category = "Style")
    void SetBarChartStyle(const FBarChartStyle& NewBarChartStyle);

    UFUNCTION(BlueprintCallable, Category = "Style")
    void SetBarSpacing(float NewBarSpacing);

    UFUNCTION(BlueprintCallable, Category = "Style")
    void SetChartPadding(FMargin NewChartPadding);

    UFUNCTION(BlueprintCallable, Category = "Chart")
    void RefreshChart();

    UFUNCTION(BlueprintPure, Category = "Chart|Hover")
    int32 GetHoveredDataPointIndex() const;

    UFUNCTION(BlueprintPure, Category = "Chart|Hover")
    FChartHoverInfo GetHoveredDataPoint() const;

    UPROPERTY(BlueprintAssignable, Category = "Simple UE Charts|Events")
    FOnChartDataPointHovered OnDataPointHovered;

    UPROPERTY(BlueprintAssignable, Category = "Simple UE Charts|Events")
    FOnChartHoverEnded OnHoverEnded;

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void SynchronizeProperties() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
    virtual const FText GetPaletteCategory() override;
#endif

private:
    void SynchronizeBarChartProperties();
    void HandleSlateHoverChanged(int32 NewHoveredDataPointIndex, FVector2D LocalPosition);

    TSharedPtr<SBarChart> MyBarChart;
    int32 HoveredDataPointIndex = INDEX_NONE;
    FChartHoverInfo HoveredDataPoint;
};
