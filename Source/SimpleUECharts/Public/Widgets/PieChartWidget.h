#pragma once

#include "Components/Widget.h"
#include "Data/ChartDataPoint.h"
#include "PieChartWidget.generated.h"

class SPieChart;

UCLASS(meta = (DisplayName = "Pie Chart"))
class SIMPLEUECHARTS_API UPieChartWidget : public UWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    TArray<FChartDataPoint> Data;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLabels = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowValues = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowPercentages = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLegend = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float StartAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "0.0"))
    float SliceSpacing = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "0.0", ClampMax = "0.95"))
    float InnerRadius = 0.0f;

    UFUNCTION(BlueprintCallable, Category = "Chart")
    void SetData(const TArray<FChartDataPoint>& NewData);

    UFUNCTION(BlueprintCallable, Category = "Chart")
    void ClearData();

    UFUNCTION(BlueprintCallable, Category = "Chart")
    void RefreshChart();

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void SynchronizeProperties() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
    virtual const FText GetPaletteCategory() override;
#endif

private:
    void SynchronizePieChartProperties();

    TSharedPtr<SPieChart> MyPieChart;
};
