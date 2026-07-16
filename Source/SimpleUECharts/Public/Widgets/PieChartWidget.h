#pragma once

#include "Components/Widget.h"
#include "Data/ChartDataPoint.h"
#include "Data/ChartStyles.h"
#include "PieChartWidget.generated.h"

class SPieChart;

UCLASS(meta = (DisplayName = "Pie Chart"))
class SIMPLEUECHARTS_API UPieChartWidget : public UWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    TArray<FChartDataPoint> Data;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    FChartStyle ChartStyle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    FPieChartStyle PieChartStyle;

    UFUNCTION(BlueprintCallable, Category = "Chart")
    void SetData(const TArray<FChartDataPoint>& NewData);

    UFUNCTION(BlueprintCallable, Category = "Chart")
    void ClearData();

    UFUNCTION(BlueprintCallable, Category = "Style")
    void SetChartStyle(const FChartStyle& NewChartStyle);

    UFUNCTION(BlueprintCallable, Category = "Style")
    void SetPieChartStyle(const FPieChartStyle& NewPieChartStyle);

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
