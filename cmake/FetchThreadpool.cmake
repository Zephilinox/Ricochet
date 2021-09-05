if (TARGET threadpool)
    return()
endif()

include(SetSystemIncludes)

if (NOT CPM_SOURCE_CACHE)
    set(CPM_SOURCE_CACHE "${PROJECT_SOURCE_DIR}/../.cpmcache/")
endif()

CPMAddPackage(
    NAME threadpool
    GITHUB_REPOSITORY Zephilinox/Threadpool
    GIT_TAG a0a52d1f2db2315abc4503c218f04a8609c16b85
    EXCLUDE_FROM_ALL "YES"
)

set_target_properties(threadpool PROPERTIES FOLDER dependencies)
set_target_includes_as_system(threadpool)