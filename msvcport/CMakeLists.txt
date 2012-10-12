cmake_minimum_required(VERSION 2.8.0)

project(fontconfig)

if (NOT WIN32 OR (MINGW OR MSYS OR CYGWIN))
  message(FATAL_ERROR
    "This CMake project is intended only for native WIN32 compilation.  "
    "For any other platforms please use the autotools ./configure script.")
endif (NOT WIN32 OR (MINGW OR MSYS OR CYGWIN))


include_directories(AFTER
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  )

file(STRINGS ../configure.ac READ_CONFIGURE_AC REGEX "^AM_INIT_AUTOMAKE *\\( *fontconfig.*")
string(REGEX REPLACE
  ".*AM_INIT_AUTOMAKE *\\( *fontconfig *, *([^ ]+) *\\).*$"
  "\\1"
  FONTCONFIG_VERSION
  "${READ_CONFIGURE_AC}"
  )
string(REGEX REPLACE
  "([0-9]+\\.[0-9]+)\\.[0-9]+"
  "\\1"
  FONTCONFIG_MAJOR_MINOR
  "${FONTCONFIG_VERSION}"
  )

if (WIN32)
  set(FONTCONFIG_DEF "${PROJECT_BINARY_DIR}/src/fontconfig.def")
  set_source_files_properties("${FONTCONFIG_DEF}" PROPERTIES GENERATED true)

  file(STRINGS ../fontconfig/fontconfig.h READ_FONTCONFIG_H  REGEX "^Fc[^ ]* *\\(")
  file(STRINGS ../src/fcdeprecate.h       READ_FCDEPRECATE_H REGEX "^Fc[^ ]* *\\(")
  file(STRINGS ../fontconfig/fcprivate.h  READ_FCPRIVATE_H   REGEX "^Fc[^ ]* *\\(")
  file(STRINGS ../fontconfig/fcfreetype.h READ_FCFREETYPE_H  REGEX "^Fc[^ ]* *\\(")

  file(WRITE "${FONTCONFIG_DEF}" "EXPORTS\n")
  foreach(line ${READ_FONTCONFIG_H} ${READ_FCDEPRECATE_H} ${READ_FCPRIVATE_H} ${READ_FCFREETYPE_H})
    string(REGEX REPLACE
      " *\\(.*$"
      ""
      FONTCONFIG_EXPORT
      "${line}"
      )
    file(APPEND "${FONTCONFIG_DEF}" "	${FONTCONFIG_EXPORT}\n")
  endforeach(line)
  file(APPEND "${FONTCONFIG_DEF}" "LIBRARY fontconfig.dll\n")
  file(APPEND "${FONTCONFIG_DEF}" "VERSION ${FONTCONFIG_MAJOR_MINOR}\n")
endif (WIN32)

set(CASEFOLDING_TXT "${PROJECT_SOURCE_DIR}/../fc-case/CaseFolding.txt")
set(FCCASE_TMPL_H "${PROJECT_SOURCE_DIR}/../fc-case/fccase.tmpl.h")
set(FCCASE_H "${PROJECT_SOURCE_DIR}/../fc-case/fccase.h")
set_source_files_properties("${FCCASE_H}" PROPERTIES GENERATED true)

if (MINGW OR MSYS)
  string(REPLACE "/" "\\\\" NATIVE_CASEFOLDING_TXT "${CASEFOLDING_TXT}")
  string(REPLACE "/" "\\\\" NATIVE_FCCASE_TMPL_H "${FCCASE_TMPL_H}")
  string(REPLACE "/" "\\\\" NATIVE_FCCASE_H "${FCCASE_H}")
else (MINGW OR MSYS)
  file(TO_NATIVE_PATH "${CASEFOLDING_TXT}" NATIVE_CASEFOLDING_TXT)
  file(TO_NATIVE_PATH "${FCCASE_TMPL_H}" NATIVE_FCCASE_TMPL_H)
  file(TO_NATIVE_PATH "${FCCASE_H}" NATIVE_FCCASE_H)
endif (MINGW OR MSYS)

add_custom_command(OUTPUT "${FCCASE_H}"
  COMMAND fc-case
  ARGS
  "${NATIVE_CASEFOLDING_TXT}" < "${NATIVE_FCCASE_TMPL_H}" > "${NATIVE_FCCASE_H}"
  DEPENDS
  fc-case
  )

set(ZAPFDINGBATS_TXT "${PROJECT_SOURCE_DIR}/../fc-glyphname/zapfdingbats.txt")
set(FCGLYPHNAME_TMPL_H "${PROJECT_SOURCE_DIR}/../fc-glyphname/fcglyphname.tmpl.h")
set(FCGLYPHNAME_H "${PROJECT_SOURCE_DIR}/../fc-glyphname/fcglyphname.h")
set_source_files_properties("${FCGLYPHNAME_H}" PROPERTIES GENERATED true)

if (MINGW OR MSYS)
  string(REPLACE "/" "\\\\" NATIVE_ZAPFDINGBATS_TXT "${ZAPFDINGBATS_TXT}")
  string(REPLACE "/" "\\\\" NATIVE_FCGLYPHNAME_TMPL_H "${FCGLYPHNAME_TMPL_H}")
  string(REPLACE "/" "\\\\" NATIVE_FCGLYPHNAME_H "${FCGLYPHNAME_H}")
else (MINGW OR MSYS)
  file(TO_NATIVE_PATH "${ZAPFDINGBATS_TXT}" NATIVE_ZAPFDINGBATS_TXT)
  file(TO_NATIVE_PATH "${FCGLYPHNAME_TMPL_H}" NATIVE_FCGLYPHNAME_TMPL_H)
  file(TO_NATIVE_PATH "${FCGLYPHNAME_H}" NATIVE_FCGLYPHNAME_H)
endif (MINGW OR MSYS)

add_custom_command(OUTPUT "${FCGLYPHNAME_H}"
  COMMAND fc-glyphname
  ARGS
  "${NATIVE_ZAPFDINGBATS_TXT}" < "${NATIVE_FCGLYPHNAME_TMPL_H}" > "${NATIVE_FCGLYPHNAME_H}"
  DEPENDS
  fc-glyphname
  )


file(STRINGS "../fc-lang/Makefile.am" READ_FCLANG_ORTH REGEX "[^ ]+\\.orth")
string(REGEX REPLACE "([ 	]+)" "" FCLANG_ORTH_LIST "${READ_FCLANG_ORTH}")
set(FCLANG_ORTH ${FCLANG_ORTH_LIST})

set(FCLANG_DIR "${PROJECT_SOURCE_DIR}/../fc-lang/")
set(FCLANG_TMPL_H "${PROJECT_SOURCE_DIR}/../fc-lang/fclang.tmpl.h")
set(FCLANG_H "${PROJECT_SOURCE_DIR}/../fc-lang/fclang.h")
set_source_files_properties("${FCLANG_H}" PROPERTIES GENERATED true)

if (MINGW OR MSYS)
  string(REPLACE "/" "\\\\" NATIVE_FCLANG_DIR "${FCLANG_DIR}")
  string(REPLACE "/" "\\\\" NATIVE_FCLANG_TMPL_H "${FCLANG_TMPL_H}")
  string(REPLACE "/" "\\\\" NATIVE_FCLANG_H "${FCLANG_H}")
else (MINGW OR MSYS)
  file(TO_NATIVE_PATH "${FCLANG_DIR}" NATIVE_FCLANG_DIR)
  file(TO_NATIVE_PATH "${FCLANG_TMPL_H}" NATIVE_FCLANG_TMPL_H)
  file(TO_NATIVE_PATH "${FCLANG_H}" NATIVE_FCLANG_H)
endif (MINGW OR MSYS)

add_custom_command(OUTPUT "${FCLANG_H}"
  COMMAND fc-lang
  ARGS
  -d "${NATIVE_FCLANG_DIR}" ${FCLANG_ORTH} < "${NATIVE_FCLANG_TMPL_H}" > "${NATIVE_FCLANG_H}"
  DEPENDS
  fc-lang "${FCCASE_H}"
  )


add_custom_target("autogenerate" ALL
  DEPENDS
  "${FCCASE_H}"
  "${FCGLYPHNAME_H}"
  "${FCLANG_H}"
  )

add_executable(fc-case
  ../fc-case/fc-case.c
  ../fc-case/fccase.tmpl.h
  ../fc-case/CaseFolding.txt
  )


add_executable(fc-lang
  "${FCCASE_H}"
  ../fc-lang/fc-lang.c
  ../fc-lang/fclang.tmpl.h
  ../fc-lang/iso-3166.txt
  ../fc-lang/iso639-1
  ../fc-lang/iso639-2
  )

add_executable(fc-glyphname
  ../fc-glyphname/fc-glyphname.c
  ../fc-glyphname/fcglyphname.tmpl.h
  )

if (WIN32)
  ADD_DEFINITIONS(-D_USE_MATH_DEFINES)
  ADD_DEFINITIONS(-DNOMINMAX)
  ADD_DEFINITIONS(-DFC_CACHEDIR="WINDOWSTEMPDIR_FONTCONFIG_CACHE")
  ADD_DEFINITIONS(-DHAVE_CONFIG_H)

  OPTION(LINKING_TO_STATIC_EXPAT_LIB "Check this if linking to static expat library" false)
  if (LINKING_TO_STATIC_EXPAT_LIB)
    ADD_DEFINITIONS(-DXML_STATIC=1)
  endif(LINKING_TO_STATIC_EXPAT_LIB)

  include_directories(AFTER
    ../msvcport
    )
endif (WIN32)


set(FREETYPE_DIR "$ENV{FREETYPE_DIR}"
  CACHE PATH "root path for freetype lib/ and include/ folders"
  )
find_path(FREETYPE_INCLUDE_DIR
  ft2build.h freetype2/freetype/freetype.h
  PATHS ${FREETYPE_DIR}/include
  )
find_library(FREETYPE_LIBRARY
  freetype libfreetype
  PATHS ${FREETYPE_DIR}/lib
  DOC "freetype library"
  )
if (FREETYPE_INCLUDE_DIR)
  include_directories(AFTER ${FREETYPE_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR}/freetype2)
endif (FREETYPE_INCLUDE_DIR)


set(EXPAT_DIR "$ENV{EXPAT_DIR}"
  CACHE PATH "root path for expat lib/ and include/ folders"
  )
find_path(EXPAT_INCLUDE_DIR
  expat.h
  PATHS ${EXPAT_DIR}/include
  )
find_library(EXPAT_LIBRARY
  expat libexpat
  PATHS ${EXPAT_DIR}/lib
  DOC "expat library"
  )
if (EXPAT_INCLUDE_DIR)
  include_directories(AFTER ${EXPAT_INCLUDE_DIR})
endif (EXPAT_INCLUDE_DIR)


set(ICONV_DIR "$ENV{ICONV_DIR}"
  CACHE PATH "root path for iconv lib/ and include/ folders"
  )
find_path(ICONV_INCLUDE_DIR
  iconv.h
  PATHS ${ICONV_DIR}/include
  )
find_library(ICONV_LIBRARY
  iconv libiconv
  PATHS ${ICONV_DIR}/lib
  DOC "iconv library"
  )
if (ICONV_INCLUDE_DIR)
  include_directories(AFTER ${ICONV_INCLUDE_DIR})
endif (ICONV_INCLUDE_DIR)


set(project_sources
  "${FCCASE_H}"
  "${FCLANG_H}"
  "${FCGLYPHNAME_H}"
  ../src/fcarch.h
  ../src/fcatomic.c
  ../src/fcblanks.c
  ../src/fccache.c
  ../src/fccfg.c
  ../src/fccharset.c
  ../src/fcdbg.c
  ../src/fcdefault.c
  ../src/fcdir.c
  ../src/fcformat.c
  ../src/fcfreetype.c
  ../src/fcfs.c
  ../src/fcinit.c
  ../src/fclang.c
  ../src/fclist.c
  ../src/fcmatch.c
  ../src/fcmatrix.c
  ../src/fcname.c
  ../src/fcpat.c
  ../src/fcserialize.c
  ../src/fcstat.c
  ../src/fcstr.c
  ../src/fcxml.c
  ../src/ftglue.h
  ../src/ftglue.c
  )

if (WIN32)
  set(project_sources
    ${project_sources}
    ../msvcport/config.h
    ../msvcport/dirent.h
    ../msvcport/fcalias.h
    ../msvcport/fcaliastail.h
    ../msvcport/fcftalias.h
    ../msvcport/fcftaliastail.h
    ../msvcport/inttypes.h
    ../msvcport/stdint.h
    ../msvcport/msvcport.h
    ../msvcport/msvcport_private.h
    ../msvcport/msvcport_private.c
    )
  find_library(LIBGCC_LIBRARY libgcc PATHS ${FFMPEG_LIBS_PATH} DOC "mingw libgcc library")
endif (WIN32)

add_library(libfontconfig STATIC ${project_sources})
add_dependencies(libfontconfig "autogenerate")

set (sharedlib_sources ${project_sources})
if (WIN32)
  set (sharedlib_sources ${sharedlib_sources} ${FONTCONFIG_DEF})
endif (WIN32)
add_library(fontconfig SHARED ${sharedlib_sources})
add_dependencies(fontconfig "autogenerate")


set(THIRD_PARTY_LIBS ${FREETYPE_LIBRARY} ${EXPAT_LIBRARY} ${ICONV_LIBRARY})
if (WIN32)
  if (LIBGCC_LIBRARY)
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} ${LIBGCC_LIBRARY})
  endif (LIBGCC_LIBRARY)
endif (WIN32)
target_link_libraries(fontconfig ${THIRD_PARTY_LIBS})


install(TARGETS libfontconfig fontconfig DESTINATION lib)
install(FILES
  ../fontconfig/fontconfig.h
  ../fontconfig/fcfreetype.h
  ../fontconfig/fcprivate.h

  DESTINATION
  include/fontconfig)

if (WIN32)
  set (FONTCONFIG_PC_IN "${PROJECT_SOURCE_DIR}/../msvcport/fontconfig.pc.in")
  set (FONTCONFIG_PC "${PROJECT_BINARY_DIR}/fontconfig.pc")
  set_source_files_properties("${FONTCONFIG_PC}" PROPERTIES GENERATED true)
  configure_file("${FONTCONFIG_PC_IN}" "${FONTCONFIG_PC}.tmp" @ONLY)
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${FONTCONFIG_PC}.tmp"
    "${FONTCONFIG_PC}")
  file(REMOVE "${FONTCONFIG_PC}.tmp")
  install(FILES "${FONTCONFIG_PC}" DESTINATION lib/pkgconfig)
endif (WIN32)
