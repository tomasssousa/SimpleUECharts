#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogSimpleUECharts, Log, All);

class FSimpleUEChartsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogSimpleUECharts, Log, TEXT("SimpleUECharts runtime module loaded."));
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogSimpleUECharts, Log, TEXT("SimpleUECharts runtime module unloaded."));
    }
};

IMPLEMENT_MODULE(FSimpleUEChartsModule, SimpleUECharts)
