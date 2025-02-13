if(WIN32)
  install(FILES
    AUTHORS
    COPYING
    NEWS
    README
    TRANSLATORS
    DESTINATION .)

  install(FILES
    GPL2.txt
    GPL3.txt
    LGPL2.1.txt
    DESTINATION .)

  install(DIRECTORY doc
    DESTINATION .)

  # mingw-w64 dlls
  #   (use msys2checkdeps.py to list required libraries / check for missing libraries)
  file(GLOB MINGW_LIBS
    ${MINGW_BIN}/LIBEAY32.dll
    ${MINGW_BIN}/SSLEAY32.dll
    ${MINGW_BIN}/libMagick*.dll
    ${MINGW_BIN}/libaspell-[0-9]*.dll
    ${MINGW_BIN}/libatk-1.0-[0-9]*.dll
    ${MINGW_BIN}/libatkmm-1.6-[0-9]*.dll
    ${MINGW_BIN}/libbrotlicommon.dll
    ${MINGW_BIN}/libbrotlidec.dll
    ${MINGW_BIN}/libbz2-[0-9]*.dll
    ${MINGW_BIN}/libcairo-[0-9]*.dll
    ${MINGW_BIN}/libcairo-gobject-[0-9]*.dll
    ${MINGW_BIN}/libcairomm-1.0-[0-9]*.dll
    ${MINGW_BIN}/libcdr-0.[0-9]*.dll
    ${MINGW_BIN}/libcrypto-1_[0-9]*.dll
    ${MINGW_BIN}/libcurl-[0-9]*.dll
    ${MINGW_BIN}/libdatrie-[0-9]*.dll
    ${MINGW_BIN}/libenchant-[0-9]*.dll
    ${MINGW_BIN}/libexpat-[0-9]*.dll
    ${MINGW_BIN}/libexslt-[0-9]*.dll
    ${MINGW_BIN}/libffi-[0-9]*.dll
    ${MINGW_BIN}/libfftw3-[0-9]*.dll
    ${MINGW_BIN}/libfontconfig-[0-9]*.dll
    ${MINGW_BIN}/libfreetype-[0-9]*.dll
    ${MINGW_BIN}/libfribidi-[0-9]*.dll
    ${MINGW_BIN}/libgailutil-[0-9][0-9].dll
    ${MINGW_BIN}/libgc-[0-9]*.dll
    ${MINGW_BIN}/libgdk-win32-2.0-[0-9]*.dll
    ${MINGW_BIN}/libgdk_pixbuf-2.0-[0-9]*.dll
    ${MINGW_BIN}/libgdkmm-2.4-[0-9]*.dll
    ${MINGW_BIN}/libgfortran-[0-9]*.dll
    ${MINGW_BIN}/libgio-2.0-[0-9]*.dll
    ${MINGW_BIN}/libgiomm-2.4-[0-9]*.dll
    ${MINGW_BIN}/libglib-2.0-[0-9]*.dll
    ${MINGW_BIN}/libglibmm-2.4-[0-9]*.dll
    ${MINGW_BIN}/libgmodule-2.0-[0-9]*.dll
    ${MINGW_BIN}/libgobject-2.0-[0-9]*.dll
    ${MINGW_BIN}/libgomp-[0-9]*.dll
    ${MINGW_BIN}/libgraphite[0-9]*.dll
    ${MINGW_BIN}/libgsl-[0-9]*.dll
    ${MINGW_BIN}/libgslcblas-[0-9]*.dll
    ${MINGW_BIN}/libgtk-win32-2.0-[0-9]*.dll
    ${MINGW_BIN}/libgtkmm-2.4-[0-9]*.dll
    ${MINGW_BIN}/libgtkspell-[0-9]*.dll
    ${MINGW_BIN}/libharfbuzz-[0-9]*.dll
    ${MINGW_BIN}/libiconv-[0-9]*.dll
    ${MINGW_BIN}/libicudt[0-9]*.dll
    ${MINGW_BIN}/libicuin[0-9]*.dll
    ${MINGW_BIN}/libicuuc[0-9]*.dll
    ${MINGW_BIN}/libidn2-[0-9]*.dll
    ${MINGW_BIN}/libintl-[0-9]*.dll
    ${MINGW_BIN}/libjasper-[0-9]*.dll
    ${MINGW_BIN}/libjpeg-[0-9]*.dll
    ${MINGW_BIN}/liblcms2-[0-9]*.dll
    ${MINGW_BIN}/liblqr-1-[0-9]*.dll
    ${MINGW_BIN}/liblzma-[0-9]*.dll
    ${MINGW_BIN}/libnghttp2-[0-9]*.dll
    ${MINGW_BIN}/libnspr[0-9]*.dll
    ${MINGW_BIN}/libopenblas.dll
    ${MINGW_BIN}/libopenjp2-[0-9]*.dll
    ${MINGW_BIN}/libpango-1.0-[0-9]*.dll
    ${MINGW_BIN}/libpangocairo-1.0-[0-9]*.dll
    ${MINGW_BIN}/libpangoft2-1.0-[0-9]*.dll
    ${MINGW_BIN}/libpangomm-1.4-[0-9]*.dll
    ${MINGW_BIN}/libpangowin32-1.0-[0-9]*.dll
    ${MINGW_BIN}/libpcre-[0-9]*.dll
    ${MINGW_BIN}/libpixman-1-[0-9]*.dll
    ${MINGW_BIN}/libplc[0-9]*.dll
    ${MINGW_BIN}/libplds[0-9]*.dll
    ${MINGW_BIN}/libpng16-[0-9]*.dll
    ${MINGW_BIN}/libpoppler-[0-9]*.dll
    ${MINGW_BIN}/libpoppler-glib-[0-9]*.dll
    ${MINGW_BIN}/libpopt-[0-9]*.dll
    ${MINGW_BIN}/libpotrace-[0-9]*.dll
    ${MINGW_BIN}/libpsl-[0-9]*.dll
    ${MINGW_BIN}/libquadmath-[0-9]*.dll
    ${MINGW_BIN}/libraqm-[0-9]*.dll
    ${MINGW_BIN}/librevenge-0.[0-9]*.dll
    ${MINGW_BIN}/librevenge-stream-0.[0-9]*.dll
    ${MINGW_BIN}/librsvg-2-[0-9]*.dll
    ${MINGW_BIN}/libsigc-2.0-[0-9]*.dll
    ${MINGW_BIN}/libsqlite3-[0-9]*.dll
    ${MINGW_BIN}/libssh2-[0-9]*.dll
    ${MINGW_BIN}/libssl-1_[0-9]*.dll
    ${MINGW_BIN}/libssp-[0-9]*.dll
    ${MINGW_BIN}/libstdc++-[0-9]*.dll
    ${MINGW_BIN}/libthai-[0-9]*.dll
    ${MINGW_BIN}/libtiff-[0-9]*.dll
    ${MINGW_BIN}/libunistring-[0-9]*.dll
    ${MINGW_BIN}/libvisio-0.[0-9]*.dll
    ${MINGW_BIN}/libwebp-[0-9]*.dll
    ${MINGW_BIN}/libwebpdemux-[0-9]*.dll
    ${MINGW_BIN}/libwebpmux-[0-9]*.dll
    ${MINGW_BIN}/libwinpthread-[0-9]*.dll
    ${MINGW_BIN}/libwpd-0.[0-9]*.dll
    ${MINGW_BIN}/libwpg-0.[0-9]*.dll
    ${MINGW_BIN}/libxml2-[0-9]*.dll
    ${MINGW_BIN}/libxslt-[0-9]*.dll
    ${MINGW_BIN}/libzstd.dll
    ${MINGW_BIN}/nss[0-9]*.dll
    ${MINGW_BIN}/nssutil[0-9]*.dll
    ${MINGW_BIN}/smime[0-9]*.dll
    ${MINGW_BIN}/tcl[0-9]*.dll
    ${MINGW_BIN}/tk[0-9]*.dll
    ${MINGW_BIN}/zlib1.dll)
  INSTALL(FILES ${MINGW_LIBS} DESTINATION .)
  # There are differences for 64-Bit and 32-Bit build environments.
  if(HAVE_MINGW64)
    install(FILES
      ${MINGW_BIN}/libgcc_s_seh-1.dll
      DESTINATION .)
  else()
    install(FILES
      ${MINGW_BIN}/libgcc_s_dw2-1.dll
      DESTINATION .)
  endif()

  # Setup application data directories, poppler files, locales, icons and themes
  file(MAKE_DIRECTORY
    data
    doc
    modules
    plugins)

  # Install hicolor/index.theme to avoid bug 1635207
  install(FILES
    ${MINGW_PATH}/share/icons/hicolor/index.theme
    DESTINATION share/icons/hicolor)

  install(DIRECTORY ${MINGW_PATH}/share/themes/MS-Windows
    DESTINATION share/themes)

  # translations for libraries (we usually shouldn't need many)
  file(GLOB inkscape_translations RELATIVE ${CMAKE_SOURCE_DIR}/po/ ${CMAKE_SOURCE_DIR}/po/*.po)
  foreach(translation ${inkscape_translations})
    get_filename_component(translation ${translation} NAME_WE)
    install(DIRECTORY ${MINGW_PATH}/share/locale/${translation}
      DESTINATION share/locale
      FILES_MATCHING
      PATTERN "*gtk20.mo"
      PATTERN "*gtkspell.mo")
  endforeach()

  install(DIRECTORY ${MINGW_PATH}/share/poppler
    DESTINATION share)

  # fontconfig
  install(DIRECTORY ${MINGW_PATH}/etc/fonts
    DESTINATION etc
    PATTERN "fonts.conf" EXCLUDE)
  install(FILES ${MINGW_PATH}/share/fontconfig/conf.avail/70-no-bitmaps.conf
    DESTINATION etc/fonts/conf.d)
  # adjust fonts.conf
  #   - add "%localappdata%\Microsoft\Windows\Fonts" as font dir
  #     which is the default path for fonts installed per-user in Windows 10 (version 1809)
  #   - store font cache in non-temporary directory in "%localappdata%\fontconfig\cache"
  set(fontdir_default    "\\t^<dir^>WINDOWSFONTDIR^</dir^>")  # the '^' are needed to escape angle brackets on Windows command shell
  set(fontdir_additional "\\t^<dir^>~/AppData/Local/Microsoft/Windows/Fonts^</dir^>")
  set(cachedir_default   "\\t^<cachedir^>/var/cache/fontconfig^</cachedir^>")
  set(cachedir_appdata   "\\t^<cachedir^>LOCAL_APPDATA_FONTCONFIG_CACHE^</cachedir^>")
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/etc/fonts/fonts.conf
    COMMAND sed 's!${fontdir_default}!${fontdir_default}\\n${fontdir_additional}!' ${MINGW_PATH}/etc/fonts/fonts.conf |
            sed 's!${cachedir_default}!${cachedir_appdata}\\n${cachedir_default}!' > ${CMAKE_BINARY_DIR}/etc/fonts/fonts.conf
    MAIN_DEPENDENCY ${MINGW_PATH}/etc/fonts/fonts.conf
  )
  add_custom_target(fonts_conf ALL DEPENDS ${CMAKE_BINARY_DIR}/etc/fonts/fonts.conf)
  install(DIRECTORY ${CMAKE_BINARY_DIR}/etc/fonts
    DESTINATION etc)

  install(DIRECTORY ${MINGW_PATH}/etc/gtk-2.0
    DESTINATION etc)

  # GTK 2.0
  install(DIRECTORY ${MINGW_LIB}/gtk-2.0
    DESTINATION lib
    FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "*.cache")

  install(DIRECTORY ${MINGW_LIB}/gdk-pixbuf-2.0
    DESTINATION lib
    FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "*.cache")

  # Aspell dictionaries
  install(DIRECTORY ${MINGW_LIB}/aspell-0.60
    DESTINATION lib)

  # Aspell backend for Enchant (gtkspell uses Enchant to access Aspell dictionaries)
  install(FILES
    ${MINGW_LIB}/enchant-2/enchant_aspell.dll
    DESTINATION lib/enchant-2)

  # tcl/tk related files (required for tkinter)
  install(DIRECTORY
    ${MINGW_PATH}/lib/tcl8
    ${MINGW_PATH}/lib/tcl8.6
    ${MINGW_PATH}/lib/tk8.6
    DESTINATION lib)

  # Necessary to run extensions on windows if it is not in the path
  if (HAVE_MINGW64)
    install(FILES
      ${MINGW_BIN}/gspawn-win64-helper.exe
      ${MINGW_BIN}/gspawn-win64-helper-console.exe
      DESTINATION .)
  else()
    install(FILES
      ${MINGW_BIN}/gspawn-win32-helper.exe
      ${MINGW_BIN}/gspawn-win32-helper-console.exe
      DESTINATION .)
  endif()

  # Python (a bit hacky for backwards compatibility with devlibs at this point)
  install(FILES
    ${MINGW_BIN}/python2.exe
    RENAME python.exe
    DESTINATION .)
  install(FILES
    ${MINGW_BIN}/python2w.exe
    RENAME pythonw.exe
    DESTINATION .)
  install(FILES
    ${MINGW_BIN}/libpython2.7.dll
    DESTINATION .)
  install(DIRECTORY ${MINGW_LIB}/python2.7
    DESTINATION lib
    PATTERN "python2.7/site-packages" EXCLUDE # specify individual packages to install below
    PATTERN "python2.7/test" EXCLUDE # we don't need the Python testsuite
    PATTERN "*.pyc" EXCLUDE
    PATTERN "*.pyo" EXCLUDE
  )

  set(site_packages "lib/python2.7/site-packages")
  # Python packages installed via pacman
  set(packages "python2-lxml" "python2-numpy" "python2-pillow")
  foreach(package ${packages})
    list_files_pacman(${package} paths)
    install_list(FILES ${paths}
      ROOT ${MINGW_PATH}
      INCLUDE ${site_packages} # only include content from "site-packages" (we might consider to install everything)
      EXCLUDE ".py[co]$"
    )
  endforeach()
  # Python packages installed via pip
  set(packages "coverage" "pyserial" "scour" "six")
  foreach(package ${packages})
    list_files_pip(${package} paths)
    install_list(FILES ${paths}
      ROOT ${MINGW_PATH}/${site_packages}
      DESTINATION ${site_packages}/
      EXCLUDE "^\\.\\.\\/" # exclude content in parent directories (notably scripts installed to /bin)
      EXCLUDE ".py[co]$"
    )
  endforeach()
  install(CODE
    "MESSAGE(\"Pre-compiling Python byte-code (.pyc files)\")
     execute_process(COMMAND \${CMAKE_INSTALL_PREFIX}/python -m compileall -qq \${CMAKE_INSTALL_PREFIX})")

endif()
