set(UniversalCan_SRC
    ProtocolMapper.cpp
    Transcievers/ELM327.cpp
    Transcievers/SerialMacOS.cpp
)
# Form the full path to the source files...
PREPEND(UniversalCan_SRC)
# ... and pass the variable to the parent scope.
set(UniversalCan_SRC ${UniversalCan_SRC}  PARENT_SCOPE)