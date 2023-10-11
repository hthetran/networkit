networkit_add_module(randomization
    AutocorrelationAnalysis.cpp
    Curveball.cpp
    CurveballGlobalTradeGenerator.cpp
    CurveballImpl.cpp
    CurveballUniformTradeGenerator.cpp
    DegreeIntervalSwitching.cpp
    DegreePreservingShuffle.cpp
    EdgeSwitching.cpp
    GlobalCurveball.cpp
    )

networkit_module_link_modules(randomization
    auxiliary graph)

add_subdirectory(test)

