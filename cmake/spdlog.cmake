# include_directories(lib/spdlog/include)
add_subdirectory(lib/fmt)
add_subdirectory(lib/spdlog)
target_compile_definitions(fmt PUBLIC FMT_CONSTEVAL=)
# target_compile_features(fmt PRIVATE cxx_std_17)
# target_compile_features(spdlog PRIVATE cxx_std_17)
# target_compile_features(spdlog::spdlog PRIVATE cxx_std_17)