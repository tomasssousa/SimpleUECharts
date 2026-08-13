#pragma once

#include "CoreMinimal.h"
#include "Fonts/SlateFontInfo.h"
#include "ChartStyles.generated.h"

UENUM(BlueprintType)
enum class EChartAxisValueMode : uint8
{
    Auto UMETA(DisplayName = "Auto"),
    Integer UMETA(DisplayName = "Integer"),
    Decimal UMETA(DisplayName = "Decimal")
};

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "0.1", UIMin = "0.1", UIMax = "3.0"))
    float ChartScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "0.1", UIMin = "0.1", UIMax = "3.0"))
    float TextScale;
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

UENUM(BlueprintType)
enum class EPieChartLegendSide : uint8
{
    Right UMETA(DisplayName = "Right"),
    Left UMETA(DisplayName = "Left")
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float PieHorizontalAlignment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLabels;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowPercentages;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLegend;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    EPieChartLegendSide LegendSide;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "64.0"))
    float LegendSpacing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart|Center Total")
    bool bShowCenterTotal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart|Center Total")
    FText CenterTotalLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart|Center Total", meta = (ClampMin = "0.1", UIMin = "0.1", UIMax = "4.0"))
    float CenterTotalValueTextScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float UnselectedOpacity;
};

USTRUCT(BlueprintType)
struct SIMPLEUECHARTS_API FTimeSeriesChartStyle
{
    GENERATED_BODY()

    FTimeSeriesChartStyle();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "0.5", UIMin = "0.5", UIMax = "8.0"))
    float LineThickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "16.0"))
    float PointRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowPointMarkers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowXAxis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowYAxis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowGridLines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowLegend;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bShowTooltips;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bIncludeZeroInYRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    bool bUseFixedYRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    double MinimumY;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    double MaximumY;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    EChartAxisValueMode YAxisValueMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "2", ClampMax = "12"))
    int32 XAxisTickCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "2", ClampMax = "12"))
    int32 YAxisTickCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FString XAxisDateFormat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor AxisColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
    FLinearColor GridLineColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart|Tooltip")
    FLinearColor TooltipBackgroundColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart|Tooltip")
    FLinearColor TooltipBorderColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart|Tooltip")
    FLinearColor TooltipTextColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float UnselectedOpacity;
};
