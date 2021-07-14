configure_file(GraphemeBreakProperty.txt GraphemeBreakProperty.txt COPYONLY)
configure_file(emoji-data.txt emoji-data.txt COPYONLY)
add_executable(unicode_properties_parse unicode_properties_parse.cpp)
