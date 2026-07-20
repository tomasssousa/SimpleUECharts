#pragma once

#include "CoreMinimal.h"
#include "Fonts/SlateFontInfo.h"
#include "ChartStyles.generated.h"

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FChartStyle
{
    GENERATED_BODY()

    FChartStyle();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor BackgroundColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor TextColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FSlateFontInfo Font;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FMargin Padding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FMargin LabelPadding;
};

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FBarChartStyle
{
    GENERATED_BODY()

    FBarChartStyle();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float BarSpacing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float MinimumBarWidth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float MaximumBarWidth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLabels;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowYAxis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowGridLines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor AxisColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor GridLineColor;
};

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FPieChartStyle
{
    GENERATED_BODY()

    FPieChartStyle();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float StartAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float SliceSpacing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float InnerRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLabels;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowPercentages;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLegend;
};
