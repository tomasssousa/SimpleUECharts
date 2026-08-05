#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Data/ChartStyles.h"
#include "Data/TimeSeriesData.h"
#include "TimeSeriesChartWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeSeriesWidgetSelectionChanged, int32, SeriesIndex);

UCLASS(BlueprintType, Blueprintable)
class SIMPLEUECHARTS_API UTimeSeriesChartWidget : public UWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chart") TArray<FTimeSeries> Series;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chart") FChartStyle ChartStyle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chart") FTimeSeriesChartStyle TimeSeriesChartStyle;
	UPROPERTY(BlueprintAssignable, Category="Chart") FOnTimeSeriesWidgetSelectionChanged OnSelectionChanged;
	UFUNCTION(BlueprintCallable, Category="Chart") void SetSeries(const TArray<FTimeSeries>& InSeries);
	UFUNCTION(BlueprintCallable, Category="Chart") void ClearSeries();
	UFUNCTION(BlueprintCallable, Category="Chart") void ClearSelection();
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
private:
	TSharedPtr<class STimeSeriesChart> MyChart;
	void HandleSelectionChanged(int32 Index);
};
