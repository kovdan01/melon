# FindSqlpp11ConnectorMysql.cmake
find_path(Sqlpp11ConnectorMysql_INCLUDE_DIR NAMES sqlpp11/mysql/mysql.h)
# Найти требуемую библиотеку в системных путях. Префиксы/суффиксы (lib*,*.so,...) подставляются автоматически.
find_library(Sqlpp11ConnectorMysql_LIBRARY NAMES sqlpp-mysql)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Sqlpp11ConnectorMysql
    Sqlpp11ConnectorMysql_INCLUDE_DIR
    Sqlpp11ConnectorMysql_LIBRARY
)

if(Sqlpp11ConnectorMysql_FOUND AND NOT TARGET sqlpp11::connector-mysql)
    # Импортированная библиотека, т.е. не собираемая этой системой сборки.
    # Тип (статическая/динамическая) не известен – может быть любым, смотря что нашлось.
    add_library(sqlpp11::connector-mysql UNKNOWN IMPORTED)
    target_include_directories(sqlpp11::connector-mysql INTERFACE "${Sqlpp11ConnectorMysql_INCLUDE_DIR}")
    set_target_properties(sqlpp11::connector-mysql PROPERTIES
        # Указать имя файла собранной внешне библиотеки.
        IMPORTED_LOCATION "${Sqlpp11ConnectorMysql_LIBRARY}"
        # Указать язык библиотеки на случай, когда она статическая.
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX")
endif()

mark_as_advanced(Sqlpp11ConnectorMysql_INCLUDE_DIR Sqlpp11ConnectorMysql_LIBRARY)
 
