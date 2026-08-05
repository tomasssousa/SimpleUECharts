#include "Slate/STimeSeriesChart.h"

#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

namespace
{
FMargin ScaledMargin(const FMargin& Margin, float Scale)
{
	return FMargin(Margin.Left * Scale, Margin.Top * Scale, Margin.Right * Scale, Margin.Bottom * Scale);
}

double GetNiceIntegerStep(double Range, int32 DesiredTickCount)
{
	const double TargetStep = Range / FMath::Max(1, DesiredTickCount - 1);
	if (TargetStep <= 1.0) return 1.0;
	const double Magnitude = FMath::Pow(10.0, FMath::FloorToDouble(FMath::LogX(10.0, TargetStep)));
	const double Normalized = TargetStep / Magnitude;
	const double NiceNormalized = Normalized <= 1.0 ? 1.0 : Normalized <= 2.0 ? 2.0 : Normalized <= 5.0 ? 5.0 : 10.0;
	return FMath::Max(1.0, NiceNormalized * Magnitude);
}

FText FormatAxisValue(double Value, EChartAxisValueMode Mode)
{
	FNumberFormattingOptions Options;
	if (Mode == EChartAxisValueMode::Integer)
	{
		Options.SetMinimumFractionalDigits(0);
		Options.SetMaximumFractionalDigits(0);
		return FText::AsNumber(FMath::RoundToInt64(Value), &Options);
	}
	Options.SetMinimumFractionalDigits(0);
	Options.SetMaximumFractionalDigits(2);
	return FText::AsNumber(Value, &Options);
}
}

void STimeSeriesChart::Construct(const FArguments& InArgs)
{
	OnSelectionChanged = InArgs._OnSelectionChanged;
	RefreshBrushes();
	RecalculateChart();
}

void STimeSeriesChart::SetSeries(const TArray<FTimeSeries>& NewSeries)
{
	Series = NewSeries;
	if (!Series.IsValidIndex(SelectedSeriesIndex)) SelectedSeriesIndex = INDEX_NONE;
	RecalculateChart();
	Invalidate(EInvalidateWidgetReason::Paint);
}

void STimeSeriesChart::ClearSeries()
{
	Series.Reset();
	SelectedSeriesIndex = INDEX_NONE;
	RecalculateChart();
	Invalidate(EInvalidateWidgetReason::Paint);
}

void STimeSeriesChart::SetChartStyle(const FChartStyle& InChartStyle)
{
	ChartStyle = InChartStyle;
	InvalidateCachedLayout();
	Invalidate(EInvalidateWidgetReason::Paint);
}

void STimeSeriesChart::SetTimeSeriesChartStyle(const FTimeSeriesChartStyle& InStyle)
{
	TimeSeriesChartStyle = InStyle;
	RefreshBrushes();
	InvalidateCachedLayout();
	Invalidate(EInvalidateWidgetReason::Paint);
}

void STimeSeriesChart::RefreshBrushes()
{
	MarkerBrush = MakeUnique<FSlateRoundedBoxBrush>(FLinearColor::White, FMath::Max(0.0f, TimeSeriesChartStyle.PointRadius));
	TooltipBrush = MakeUnique<FSlateRoundedBoxBrush>(TimeSeriesChartStyle.TooltipBackgroundColor, 4.0f,
		TimeSeriesChartStyle.TooltipBorderColor, 1.0f);
}

void STimeSeriesChart::ClearSelection()
{
	if (SelectedSeriesIndex == INDEX_NONE) return;
	SelectedSeriesIndex = INDEX_NONE;
	Invalidate(EInvalidateWidgetReason::Paint);
	OnSelectionChanged.ExecuteIfBound(INDEX_NONE);
}

void STimeSeriesChart::RecalculateChart()
{
	InvalidateCachedLayout();
}

void STimeSeriesChart::InvalidateCachedLayout()
{
	bLayoutDirty = true;
	CachedLocalSize = FVector2D(-1.0f, -1.0f);
	CachedSeries.Reset(); CachedXTicks.Reset(); CachedYTicks.Reset(); CachedLegendEntries.Reset();
}

FSlateFontInfo STimeSeriesChart::GetChartFont() const
{
	FSlateFontInfo Result = ChartStyle.Font;
	if (Result.FontObject == nullptr && Result.TypefaceFontName.IsNone()) Result = FCoreStyle::GetDefaultFontStyle("Regular", 10);
	Result.Size = FMath::Max(1, FMath::RoundToInt(Result.Size * FMath::Max(0.1f, ChartStyle.TextScale)));
	return Result;
}

void STimeSeriesChart::EnsureCachedLayout(const FVector2D& LocalSize) const
{
	if (!bLayoutDirty && CachedLocalSize.Equals(LocalSize, KINDA_SMALL_NUMBER)) return;
	bLayoutDirty = false; CachedLocalSize = LocalSize;
	CachedSeries.Reset(); CachedXTicks.Reset(); CachedYTicks.Reset(); CachedLegendEntries.Reset();
	if (Series.IsEmpty() || LocalSize.X <= 0.0f || LocalSize.Y <= 0.0f) return;

	FDateTime MinTime = FDateTime::MaxValue(), MaxTime = FDateTime::MinValue();
	double MinY = DBL_MAX, MaxY = -DBL_MAX;
	for (int32 SeriesIndex = 0; SeriesIndex < Series.Num(); ++SeriesIndex)
	{
		FCachedSeries& Cached = CachedSeries.AddDefaulted_GetRef();
		Cached.SourceSeriesIndex = SeriesIndex; Cached.Name = Series[SeriesIndex].Name; Cached.Color = Series[SeriesIndex].Color;
		for (int32 PointIndex = 0; PointIndex < Series[SeriesIndex].Points.Num(); ++PointIndex)
		{
			const FTimeSeriesPoint& Point = Series[SeriesIndex].Points[PointIndex];
			if (Point.Timestamp.GetTicks() <= 0 || !FMath::IsFinite(Point.Value)) continue;
			FCachedPoint& Out = Cached.Points.AddDefaulted_GetRef();
			Out.SourcePointIndex = PointIndex; Out.Timestamp = Point.Timestamp; Out.Value = Point.Value; Out.LabelOverride = Point.LabelOverride;
			MinTime = FMath::Min(MinTime, Point.Timestamp); MaxTime = FMath::Max(MaxTime, Point.Timestamp);
			MinY = FMath::Min(MinY, Point.Value); MaxY = FMath::Max(MaxY, Point.Value);
		}
		Cached.Points.Sort([](const FCachedPoint& A, const FCachedPoint& B) { return A.Timestamp < B.Timestamp; });
	}
	if (MinY == DBL_MAX || MinTime == FDateTime::MaxValue()) return;
	if (TimeSeriesChartStyle.bUseFixedYRange) { MinY = TimeSeriesChartStyle.MinimumY; MaxY = TimeSeriesChartStyle.MaximumY; }
	else if (TimeSeriesChartStyle.bIncludeZeroInYRange) { MinY = FMath::Min(0.0, MinY); MaxY = FMath::Max(0.0, MaxY); }
	if (FMath::IsNearlyEqual(MinY, MaxY)) MaxY = MinY + 1.0;
	const int32 DesiredYTickCount = FMath::Max(2, TimeSeriesChartStyle.YAxisTickCount);
	double IntegerTickStep = 0.0;
	if (TimeSeriesChartStyle.YAxisValueMode == EChartAxisValueMode::Integer)
	{
		IntegerTickStep = GetNiceIntegerStep(MaxY - MinY, DesiredYTickCount);
		MinY = FMath::FloorToDouble(MinY / IntegerTickStep) * IntegerTickStep;
		MaxY = FMath::CeilToDouble(MaxY / IntegerTickStep) * IntegerTickStep;
		if (FMath::IsNearlyEqual(MinY, MaxY)) MaxY = MinY + IntegerTickStep;
	}
	if (MinTime == MaxTime) MaxTime += FTimespan::FromDays(1.0);

	const FSlateFontInfo Font = GetChartFont();
	const TSharedRef<FSlateFontMeasure> Measure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	const float FontHeight = Measure->GetMaxCharacterHeight(Font);
	float MaxYLabelWidth = 0.0f;
	const int32 YTickCount = TimeSeriesChartStyle.YAxisValueMode == EChartAxisValueMode::Integer
		? FMath::Max(2, FMath::RoundToInt((MaxY - MinY) / IntegerTickStep) + 1)
		: DesiredYTickCount;
	for (int32 I = 0; I < YTickCount; ++I)
	{
		const double Alpha = static_cast<double>(I) / (YTickCount - 1);
		MaxYLabelWidth = FMath::Max(MaxYLabelWidth, Measure->Measure(FormatAxisValue(MinY + (MaxY - MinY) * Alpha, TimeSeriesChartStyle.YAxisValueMode), Font).X);
	}
	const FMargin Padding = ScaledMargin(ChartStyle.Padding, FMath::Max(0.1f, ChartStyle.ChartScale));
	const float LegendHeight = TimeSeriesChartStyle.bShowLegend ? FontHeight + 16.0f : 0.0f;
	CachedPlotTopLeft = FVector2D(Padding.Left + (TimeSeriesChartStyle.bShowYAxis ? MaxYLabelWidth + 10.0f : 0.0f), Padding.Top);
	CachedPlotBottomRight = FVector2D(LocalSize.X - Padding.Right, LocalSize.Y - Padding.Bottom - (TimeSeriesChartStyle.bShowXAxis ? FontHeight + 10.0f : 0.0f) - LegendHeight);
	if (CachedPlotBottomRight.X <= CachedPlotTopLeft.X || CachedPlotBottomRight.Y <= CachedPlotTopLeft.Y) return;
	const int64 TimeSpanTicks = MaxTime.GetTicks() - MinTime.GetTicks();
	const float PlotWidth = CachedPlotBottomRight.X - CachedPlotTopLeft.X, PlotHeight = CachedPlotBottomRight.Y - CachedPlotTopLeft.Y;
	for (FCachedSeries& Cached : CachedSeries)
	{
		for (FCachedPoint& Point : Cached.Points)
		{
			const double XAlpha = static_cast<double>(Point.Timestamp.GetTicks() - MinTime.GetTicks()) / TimeSpanTicks;
			const double YAlpha = (Point.Value - MinY) / (MaxY - MinY);
			Point.Position = FVector2D(CachedPlotTopLeft.X + PlotWidth * XAlpha, CachedPlotBottomRight.Y - PlotHeight * YAlpha);
		}
	}
	for (int32 I = 0; I < YTickCount; ++I)
	{
		const double Alpha = static_cast<double>(I) / (YTickCount - 1);
		FCachedTick& Tick = CachedYTicks.AddDefaulted_GetRef(); Tick.Position = CachedPlotBottomRight.Y - PlotHeight * Alpha;
		Tick.Text = FormatAxisValue(MinY + (MaxY - MinY) * Alpha, TimeSeriesChartStyle.YAxisValueMode);
		Tick.TextSize = Measure->Measure(Tick.Text, Font); Tick.TextPosition = FVector2D(CachedPlotTopLeft.X - Tick.TextSize.X - 8.0f, Tick.Position - Tick.TextSize.Y * 0.5f);
	}
	const int32 XTickCount = FMath::Max(2, TimeSeriesChartStyle.XAxisTickCount);
	for (int32 I = 0; I < XTickCount; ++I)
	{
		const double Alpha = static_cast<double>(I) / (XTickCount - 1);
		FCachedTick& Tick = CachedXTicks.AddDefaulted_GetRef(); Tick.Position = CachedPlotTopLeft.X + PlotWidth * Alpha;
		const FDateTime Date(MinTime.GetTicks() + FMath::RoundToInt64(TimeSpanTicks * Alpha));
		Tick.Text = FText::FromString(Date.ToFormattedString(*TimeSeriesChartStyle.XAxisDateFormat));
		Tick.TextSize = Measure->Measure(Tick.Text, Font); Tick.TextPosition = FVector2D(Tick.Position - Tick.TextSize.X * 0.5f, CachedPlotBottomRight.Y + 7.0f);
	}
	if (TimeSeriesChartStyle.bShowLegend)
	{
		float TotalWidth = 0.0f;
		for (const FCachedSeries& Item : CachedSeries) TotalWidth += 34.0f + Measure->Measure(Item.Name, Font).X;
		float X = FMath::Max(Padding.Left, (LocalSize.X - TotalWidth) * 0.5f);
		const float Y = LocalSize.Y - Padding.Bottom - FontHeight;
		for (const FCachedSeries& Item : CachedSeries)
		{
			const FVector2D TextSize = Measure->Measure(Item.Name, Font);
			FCachedLegendEntry& Entry = CachedLegendEntries.AddDefaulted_GetRef(); Entry.SourceSeriesIndex = Item.SourceSeriesIndex; Entry.Text = Item.Name; Entry.Color = Item.Color;
			Entry.LineStart = FVector2D(X, Y + FontHeight * 0.5f); Entry.LineEnd = FVector2D(X + 18.0f, Entry.LineStart.Y); Entry.TextPosition = FVector2D(X + 24.0f, Y); Entry.TextSize = TextSize;
			Entry.HitRect = FSlateRect(X - 4.0f, Y - 4.0f, X + 28.0f + TextSize.X, Y + FontHeight + 4.0f); X = Entry.HitRect.Right + 10.0f;
		}
	}
}

FVector2D STimeSeriesChart::ComputeDesiredSize(float) const { return FVector2D(640.0f, 300.0f); }

int32 STimeSeriesChart::OnPaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& CullingRect, FSlateWindowElementList& Out, int32 LayerId, const FWidgetStyle& WidgetStyle, bool bParentEnabled) const
{
	EnsureCachedLayout(Geometry.GetLocalSize());
	const FSlateBrush* White = FCoreStyle::Get().GetBrush("WhiteBrush");
	FSlateDrawElement::MakeBox(Out, LayerId++, Geometry.ToPaintGeometry(), White, ESlateDrawEffect::None, ChartStyle.BackgroundColor);
	const FSlateFontInfo Font = GetChartFont();
	if (TimeSeriesChartStyle.bShowGridLines) for (const FCachedTick& Tick : CachedYTicks)
		FSlateDrawElement::MakeLines(Out, LayerId, Geometry.ToPaintGeometry(), { FVector2D(CachedPlotTopLeft.X, Tick.Position), FVector2D(CachedPlotBottomRight.X, Tick.Position) }, ESlateDrawEffect::None, TimeSeriesChartStyle.GridLineColor, true, 1.0f);
	LayerId++;
	if (TimeSeriesChartStyle.bShowYAxis) for (const FCachedTick& Tick : CachedYTicks) FSlateDrawElement::MakeText(Out, LayerId, Geometry.ToPaintGeometry(Tick.TextPosition, Tick.TextSize), Tick.Text, Font, ESlateDrawEffect::None, ChartStyle.TextColor);
	if (TimeSeriesChartStyle.bShowXAxis) for (const FCachedTick& Tick : CachedXTicks) FSlateDrawElement::MakeText(Out, LayerId, Geometry.ToPaintGeometry(Tick.TextPosition, Tick.TextSize), Tick.Text, Font, ESlateDrawEffect::None, ChartStyle.TextColor);
	LayerId++;
	for (const FCachedSeries& Item : CachedSeries)
	{
		if (Item.Points.IsEmpty()) continue;
		FLinearColor Color = Item.Color;
		if (SelectedSeriesIndex != INDEX_NONE && SelectedSeriesIndex != Item.SourceSeriesIndex) Color.A *= TimeSeriesChartStyle.UnselectedOpacity;
		TArray<FVector2D> Positions; Positions.Reserve(Item.Points.Num()); for (const FCachedPoint& Point : Item.Points) Positions.Add(Point.Position);
		if (Positions.Num() > 1) FSlateDrawElement::MakeLines(Out, LayerId, Geometry.ToPaintGeometry(), Positions, ESlateDrawEffect::None, Color, true, TimeSeriesChartStyle.LineThickness);
		if (TimeSeriesChartStyle.bShowPointMarkers)
		{
			const float D = TimeSeriesChartStyle.PointRadius * 2.0f;
			for (const FCachedPoint& Point : Item.Points) FSlateDrawElement::MakeBox(Out, LayerId + 1, Geometry.ToPaintGeometry(Point.Position - FVector2D(D * 0.5f), FVector2D(D)), MarkerBrush.Get(), ESlateDrawEffect::None, Color);
		}
	}
	LayerId += 2;
	for (const FCachedLegendEntry& Entry : CachedLegendEntries)
	{
		FLinearColor Color = Entry.Color; if (SelectedSeriesIndex != INDEX_NONE && SelectedSeriesIndex != Entry.SourceSeriesIndex) Color.A *= TimeSeriesChartStyle.UnselectedOpacity;
		FSlateDrawElement::MakeLines(Out, LayerId, Geometry.ToPaintGeometry(), {Entry.LineStart, Entry.LineEnd}, ESlateDrawEffect::None, Color, true, TimeSeriesChartStyle.LineThickness);
		FLinearColor LegendTextColor = ChartStyle.TextColor; if (SelectedSeriesIndex != INDEX_NONE && SelectedSeriesIndex != Entry.SourceSeriesIndex) LegendTextColor.A *= TimeSeriesChartStyle.UnselectedOpacity;
		FSlateDrawElement::MakeText(Out, LayerId, Geometry.ToPaintGeometry(Entry.TextPosition, Entry.TextSize), Entry.Text, Font, ESlateDrawEffect::None, LegendTextColor);
	}
	if (TimeSeriesChartStyle.bShowTooltips && CachedSeries.IsValidIndex(HoveredSeriesIndex) && CachedSeries[HoveredSeriesIndex].Points.IsValidIndex(HoveredPointIndex))
	{
		const FCachedSeries& Item = CachedSeries[HoveredSeriesIndex]; const FCachedPoint& Point = Item.Points[HoveredPointIndex];
		const FText DateText = FText::FromString(Point.Timestamp.ToFormattedString(*TimeSeriesChartStyle.XAxisDateFormat));
		const FText ValueText = FText::Format(NSLOCTEXT("TimeSeriesChart", "TooltipValue", "{0}: {1}"), Item.Name, FormatAxisValue(Point.Value, TimeSeriesChartStyle.YAxisValueMode));
		const TSharedRef<FSlateFontMeasure> TooltipMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		const FVector2D DateSize = TooltipMeasure->Measure(DateText, Font); const FVector2D ValueSize = TooltipMeasure->Measure(ValueText, Font);
		const float RowGap = 3.0f; const FVector2D ContentSize(FMath::Max(DateSize.X, ValueSize.X), DateSize.Y + RowGap + ValueSize.Y);
		FVector2D Pos = Point.Position + FVector2D(10.0f, -ContentSize.Y - 18.0f);
		Pos.X = FMath::Clamp(Pos.X, 8.0f, FMath::Max(8.0f, Geometry.GetLocalSize().X - ContentSize.X - 20.0f));
		Pos.Y = FMath::Clamp(Pos.Y, 8.0f, FMath::Max(8.0f, Geometry.GetLocalSize().Y - ContentSize.Y - 20.0f));
		const FLinearColor TooltipFill = TooltipBrush->GetTint(WidgetStyle) * WidgetStyle.GetColorAndOpacityTint();
		FSlateDrawElement::MakeBox(Out, LayerId, Geometry.ToPaintGeometry(Pos - FVector2D(6.0f), ContentSize + FVector2D(12.0f)),
			TooltipBrush.Get(), ESlateDrawEffect::None, TooltipFill);
		FSlateDrawElement::MakeText(Out, LayerId + 1, Geometry.ToPaintGeometry(Pos, DateSize), DateText, Font, ESlateDrawEffect::None, TimeSeriesChartStyle.TooltipTextColor);
		FSlateDrawElement::MakeText(Out, LayerId + 1, Geometry.ToPaintGeometry(Pos + FVector2D(0.0f, DateSize.Y + RowGap), ValueSize), ValueText, Font, ESlateDrawEffect::None, TimeSeriesChartStyle.TooltipTextColor);
		LayerId += 2;
	}
	return LayerId;
}

float STimeSeriesChart::DistanceToSegment(const FVector2D& P, const FVector2D& A, const FVector2D& B)
{
	const FVector2D AB = B - A; const float L2 = AB.SizeSquared(); if (L2 <= KINDA_SMALL_NUMBER) return FVector2D::Distance(P, A);
	const float T = FMath::Clamp(FVector2D::DotProduct(P - A, AB) / L2, 0.0f, 1.0f); return FVector2D::Distance(P, A + AB * T);
}

int32 STimeSeriesChart::HitTestSeries(const FVector2D& P) const
{
	if (SelectedSeriesIndex != INDEX_NONE)
	{
		for (const FCachedLegendEntry& Entry : CachedLegendEntries) if (Entry.SourceSeriesIndex == SelectedSeriesIndex && Entry.HitRect.ContainsPoint(P)) return SelectedSeriesIndex;
		for (const FCachedSeries& Item : CachedSeries) if (Item.SourceSeriesIndex == SelectedSeriesIndex)
			for (int32 I = 1; I < Item.Points.Num(); ++I) if (DistanceToSegment(P, Item.Points[I - 1].Position, Item.Points[I].Position) < 10.0f) return SelectedSeriesIndex;
	}
	for (const FCachedLegendEntry& Entry : CachedLegendEntries) if (Entry.HitRect.ContainsPoint(P)) return Entry.SourceSeriesIndex;
	float Best = 10.0f; int32 Result = INDEX_NONE;
	for (const FCachedSeries& Item : CachedSeries) for (int32 I = 1; I < Item.Points.Num(); ++I) { const float D = DistanceToSegment(P, Item.Points[I - 1].Position, Item.Points[I].Position); if (D < Best) { Best = D; Result = Item.SourceSeriesIndex; } }
	return Result;
}

FReply STimeSeriesChart::OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event)
{
	if (Event.GetEffectingButton() != EKeys::LeftMouseButton) return FReply::Unhandled();
	EnsureCachedLayout(Geometry.GetLocalSize()); const int32 Hit = HitTestSeries(Geometry.AbsoluteToLocal(Event.GetScreenSpacePosition()));
	if (Hit == INDEX_NONE) { if (SelectedSeriesIndex != INDEX_NONE) ClearSelection(); return FReply::Handled(); }
	SelectedSeriesIndex = SelectedSeriesIndex == Hit ? INDEX_NONE : Hit; Invalidate(EInvalidateWidgetReason::Paint); OnSelectionChanged.ExecuteIfBound(SelectedSeriesIndex); return FReply::Handled();
}

FReply STimeSeriesChart::OnMouseMove(const FGeometry& Geometry, const FPointerEvent& Event)
{
	EnsureCachedLayout(Geometry.GetLocalSize()); const FVector2D P = Geometry.AbsoluteToLocal(Event.GetScreenSpacePosition()); float Best = 12.0f; int32 BestSeries = INDEX_NONE, BestPoint = INDEX_NONE;
	for (int32 S = 0; S < CachedSeries.Num(); ++S) for (int32 I = 0; I < CachedSeries[S].Points.Num(); ++I) { const float D = FVector2D::Distance(P, CachedSeries[S].Points[I].Position); if (D < Best) { Best = D; BestSeries = S; BestPoint = I; } }
	if (BestSeries != HoveredSeriesIndex || BestPoint != HoveredPointIndex) { HoveredSeriesIndex = BestSeries; HoveredPointIndex = BestPoint; Invalidate(EInvalidateWidgetReason::Paint); }
	return FReply::Handled();
}

void STimeSeriesChart::OnMouseLeave(const FPointerEvent& Event)
{
	HoveredSeriesIndex = HoveredPointIndex = INDEX_NONE; Invalidate(EInvalidateWidgetReason::Paint); SLeafWidget::OnMouseLeave(Event);
}
