set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

FIND_PACKAGE(Qt5LinguistTools)

file(GLOB TRANSLATION_FILES ${CMAKE_CURRENT_LIST_DIR}/*.ts)
set_source_files_properties(${TRANSLATION_FILES} PROPERTIES OUTPUT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR})

qt5_create_translation(TRANSLATION_MESSAGES ${CMAKE_CURRENT_SOURCE_DIR}/../../src ${TRANSLATION_FILES})
qt5_add_translation(TRANSLATION_QM ${TRANSLATION_FILES})
add_custom_target(translations_update DEPENDS ${TRANSLATION_MESSAGES})
add_custom_target(translations DEPENDS ${TRANSLATION_QM})

