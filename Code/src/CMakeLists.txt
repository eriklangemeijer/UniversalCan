set(UniversalCan_SRC
    CanValue.cpp
    Transcievers/ELM327.cpp
    CanMessage.cpp
    CanMessageTemplate.cpp
    CanValueTemplate.cpp
    ModifierFunction.cpp
    ProtocolDefinitionParser.cpp
)
if(WIN32)
    list(APPEND UniversalCan_SRC Transcievers/SerialWindows.cpp)
else()
    list(APPEND UniversalCan_SRC Transcievers/SerialUnix.cpp)
endif()
# Form the full path to the source files...
PREPEND(UniversalCan_SRC)
# ... and pass the variable to the parent scope.
set(UniversalCan_SRC ${UniversalCan_SRC}  PARENT_SCOPE)