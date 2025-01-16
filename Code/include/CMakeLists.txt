set(UniversalCan_INC
    CanMessage.h
    CanMessageTemplate.h
    CanValue.h
    CanValueTemplate.h
    ModifierFunction.h
    ProtocolMapper.h
    ProtocolDefinitionParser.h
    Transcievers/ELM327.h
    Transcievers/ISerial.h
    Transcievers/ITransciever.h
    )
if(WIN32)
    list(APPEND UniversalCan_INC Transcievers/SerialWindows.h)
else()
    list(APPEND UniversalCan_INC Transcievers/SerialUnix.h)
endif()
include_directories(Transcievers)
PREPEND(UniversalCan_INC)
set(UniversalCan_INC ${UniversalCan_INC}  PARENT_SCOPE)