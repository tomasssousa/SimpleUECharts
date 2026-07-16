#pragma once

#include "CoreMinimal.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/CoreStyle.h"
#include "ChartStyles.generated.h"

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FChartStyle
{
    GENERATED_BODY()

    FChartStyle()
        : BackgroundColor(FLinearColor::Transparent)
        , TextColor(FLinearColor::White)
        , Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 10))
        , Padding(8.0f)
        , LabelPadding(4.0f)
    {
    }

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

    FBarChartStyle()
        : BarSpacing(8.0f)
        , MinimumBarWidth(8.0f)
        , MaximumBarWidth(128.0f)
        , bShowLabels(true)
        , bShowValues(true)
        , bShowYAxis(true)
        , bShowGridLines(false)
        , AxisColor(FLinearColor::White)
        , GridLineColor(0.25f, 0.25f, 0.25f, 1.0f)
    {
    }

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
