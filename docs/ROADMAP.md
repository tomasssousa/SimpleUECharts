# SimpleUECharts Roadmap

## Current

- Runtime-only Unreal-native bar, pie, and time-series charts.
- UMG wrappers over cached Slate drawing.
- Event-driven data/style updates with no chart Tick requirement.
- Generic labels, values, timestamps, series, colors, and interaction events.

## Next

- Optional axis titles and value/date formatter delegates.
- Configurable tooltip presentation.
- Accessibility and keyboard selection.
- Large-series decimation for dense sensor history.

## Boundaries

Application data aggregation and business rules stay outside this plugin. The plugin receives prepared chart data and does not depend on GoTwin models, HTTP controllers, or UI styles.
