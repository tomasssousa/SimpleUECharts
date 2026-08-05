#include "Widgets/TimeSeriesChartWidget.h"
#include "Slate/STimeSeriesChart.h"

TSharedRef<SWidget> UTimeSeriesChartWidget::RebuildWidget()
{
	SAssignNew(MyChart, STimeSeriesChart).OnSelectionChanged(FOnTimeSeriesSelectionChanged::CreateUObject(this, &UTimeSeriesChartWidget::HandleSelectionChanged));
	return MyChart.ToSharedRef();
}
void UTimeSeriesChartWidget::SynchronizeProperties() { Super::SynchronizeProperties(); if (MyChart) { MyChart->SetChartStyle(ChartStyle); MyChart->SetTimeSeriesChartStyle(TimeSeriesChartStyle); MyChart->SetSeries(Series); } }
void UTimeSeriesChartWidget::ReleaseSlateResources(bool bReleaseChildren) { Super::ReleaseSlateResources(bReleaseChildren); MyChart.Reset(); }
void UTimeSeriesChartWidget::SetSeries(const TArray<FTimeSeries>& InSeries) { Series = InSeries; if (MyChart) MyChart->SetSeries(Series); }
void UTimeSeriesChartWidget::ClearSeries() { Series.Reset(); if (MyChart) MyChart->ClearSeries(); }
void UTimeSeriesChartWidget::ClearSelection() { if (MyChart) MyChart->ClearSelection(); }
void UTimeSeriesChartWidget::HandleSelectionChanged(int32 Index) { OnSelectionChanged.Broadcast(Index); }
