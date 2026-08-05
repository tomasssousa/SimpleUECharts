#pragma once

#include "CoreMinimal.h"
#include "TimeSeriesData.generated.h"

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FTimeSeriesPoint
{
    GENERATED_BODY()

    FTimeSeriesPoint() = default;
    FTimeSeriesPoint(const FDateTime& InTimestamp, double InValue, const FText& InLabelOverride = FText::GetEmpty())
        : Timestamp(InTimestamp)
        , Value(InValue)
        , LabelOverride(InLabelOverride)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    double Value = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FText LabelOverride;
};

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FTimeSeries
{
    GENERATED_BODY()

    FTimeSeries() = default;
    FTimeSeries(const FText& InName, const FLinearColor& InColor, const TArray<FTimeSeriesPoint>& InPoints)
        : Name(InName)
        , Color(InColor)
        , Points(InPoints)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor Color = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    TArray<FTimeSeriesPoint> Points;
};
