# This is called by cmake as an external process from
# ./src/CMakeLists.txt and creates inkscape-version.cpp
#
# It's also included directly in ./CMakeScripts/Dist.cmake to
# determine INKSCAPE_REVISION, INKSCAPE_REVISION_HASH and INKSCAPE_REVISION_DATE
# for the 'dist' targets
#
# These variables are defined by the caller, matching the CMake equivilents.
# - ${INKSCAPE_SOURCE_DIR}
# - ${INKSCAPE_BINARY_DIR}

set(INKSCAPE_REVISION "2060ec1f9f, 2020-04-08")

if(EXISTS ${INKSCAPE_SOURCE_DIR}/.git)
    execute_process(COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${INKSCAPE_SOURCE_DIR}
        OUTPUT_VARIABLE INKSCAPE_REVISION_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND git log -n 1 --pretty=%cd --date=short
        WORKING_DIRECTORY ${INKSCAPE_SOURCE_DIR}
        OUTPUT_VARIABLE INKSCAPE_REVISION_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(INKSCAPE_REVISION "${INKSCAPE_REVISION_HASH}, ${INKSCAPE_REVISION_DATE}")

    execute_process(COMMAND
        git status -s ${INKSCAPE_SOURCE_DIR}/src
        WORKING_DIRECTORY ${INKSCAPE_SOURCE_DIR}
        OUTPUT_VARIABLE INKSCAPE_SOURCE_MODIFIED
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT INKSCAPE_SOURCE_MODIFIED STREQUAL "")
        set(INKSCAPE_REVISION "${INKSCAPE_REVISION}, custom")
    endif()
elseif(EXISTS ${INKSCAPE_SOURCE_DIR}/debian/git-build-recipe.manifest)
    # workaround for debian packaging in ppa (where we have no repo)
    # TODO: figure out how to match this to standard build environments
    execute_process(COMMAND sed -n "s/.*deb-version\\s*//p" git-build-recipe.manifest
        WORKING_DIRECTORY ${INKSCAPE_SOURCE_DIR}/debian
        OUTPUT_VARIABLE DEB_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(INKSCAPE_REVISION "${DEB_VERSION}")
endif()

if(NOT "${INKSCAPE_BINARY_DIR}" STREQUAL "")
    message("revision is " ${INKSCAPE_REVISION})
    configure_file(${INKSCAPE_SOURCE_DIR}/src/inkscape-version.cpp.in ${INKSCAPE_BINARY_DIR}/src/inkscape-version.cpp)
endif()
