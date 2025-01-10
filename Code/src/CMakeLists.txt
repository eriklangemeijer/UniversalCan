set(UniversalCan_SRC
    Transcievers/ELM327.cpp
    CanMessage.cpp
    CanMessageTemplate.cpp
    CanValueTemplate.cpp
    ProtocolDefinitionParser.cpp
)
if(WIN32)
    list(APPEND UniversalCan_SRC Transcievers/SerialWindows.cpp)
elseif(APPLE)
    list(APPEND UniversalCan_SRC Transcievers/SerialMacOS.cpp)
endif()
# Form the full path to the source files...
PREPEND(UniversalCan_SRC)
# ... and pass the variable to the parent scope.
set(UniversalCan_SRC ${UniversalCan_SRC}  PARENT_SCOPE)