#pragma once

#include "CoreMinimal.h"
#include "Fonts/SlateFontInfo.h"
#include "ChartStyles.generated.h"

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FChartStyle
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor BackgroundColor = FLinearColor::Transparent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor TextColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FSlateFontInfo Font;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FMargin Padding = FMargin(8.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FMargin LabelPadding = FMargin(4.0f);
};

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FBarChartStyle
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float BarSpacing = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float MinimumBarWidth = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float MaximumBarWidth = 128.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLabels = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowValues = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowYAxis = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowGridLines = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor AxisColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor GridLineColor = FLinearColor(0.25f, 0.25f, 0.25f, 1.0f);
};

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FPieChartStyle
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float StartAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float SliceSpacing = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    float InnerRadius = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLabels = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowValues = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowPercentages = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLegend = false;
};
