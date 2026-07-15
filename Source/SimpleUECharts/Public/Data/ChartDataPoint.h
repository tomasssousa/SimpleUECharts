#pragma once

#include "CoreMinimal.h"
#include "ChartDataPoint.generated.h"

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FChartDataPoint
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FText Label;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float Value = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor Color = FLinearColor::White;
};
