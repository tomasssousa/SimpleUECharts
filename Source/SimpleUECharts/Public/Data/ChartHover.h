#pragma once

#include "CoreMinimal.h"
#include "ChartHover.generated.h"

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FChartHoverInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Simple UE Charts|Hover")
    int32 DataPointIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly, Category = "Simple UE Charts|Hover")
    FText Label;

    UPROPERTY(BlueprintReadOnly, Category = "Simple UE Charts|Hover")
    float Value = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Simple UE Charts|Hover")
    float Percentage = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Simple UE Charts|Hover")
    FVector2D LocalPosition = FVector2D::ZeroVector;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnChartDataPointHovered,
    const FChartHoverInfo&,
    HoverInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChartHoverEnded);

DECLARE_DELEGATE_TwoParams(FOnSlateChartHoverChanged, int32, FVector2D);
