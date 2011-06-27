IF (NOT QT_USE_FILE)
	CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
	IF (COMMAND cmake_policy)
		cmake_policy (SET CMP0003 NEW)
	ENDIF (COMMAND cmake_policy)

	PROJECT (leechcraft_advancednotifications)

	IF (NOT CMAKE_MODULE_PATH)
		SET (CMAKE_MODULE_PATH "/usr/local/share/leechcraft/cmake;/usr/share/leechcraft/cmake")
	ENDIF (NOT CMAKE_MODULE_PATH)

	FIND_PACKAGE (Boost REQUIRED)
	FIND_PACKAGE (Qt4 REQUIRED)
	FIND_PACKAGE (LeechCraft REQUIRED)
ENDIF (NOT QT_USE_FILE)

INCLUDE (${QT_USE_FILE})
INCLUDE_DIRECTORIES (
	${CMAKE_CURRENT_BINARY_DIR}
	${Boost_INCLUDE_DIR}
	${LEECHCRAFT_INCLUDE_DIR}
	)
SET (SRCS
	advancednotifications.cpp
	generalhandler.cpp
	concretehandlerbase.cpp
	systemtrayhandler.cpp
	handlersconfigurator.cpp
	)
SET (HEADERS
	advancednotifications.h
	generalhandler.h
	concretehandlerbase.h
	systemtrayhandler.h
	handlersconfigurator.h
	)
QT4_WRAP_CPP (MOC_SRCS ${HEADERS})

ADD_LIBRARY (leechcraft_advancednotifications SHARED
	${COMPILED_TRANSLATIONS}
	${SRCS}
	${MOC_SRCS}
	)
TARGET_LINK_LIBRARIES (leechcraft_advancednotifications
	${QT_LIBRARIES}
	${LEECHCRAFT_LIBRARIES}
	)
INSTALL (TARGETS leechcraft_advancednotifications DESTINATION ${LC_PLUGINS_DEST})