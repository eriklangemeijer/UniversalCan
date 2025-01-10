set(UniversalCan_INC
    CanMessage.h
    CanMessageTemplate.h
    CanValueTemplate.h
    ProtocolMapper.h
    ProtocolDefinitionParser.h
    Transcievers/ELM327.h
    Transcievers/ISerial.h
    Transcievers/ITransciever.h
    )
if(WIN32)
    list(APPEND UniversalCan_INC Transcievers/SerialWindows.h)
elseif(APPLE)
    list(APPEND UniversalCan_INC Transcievers/SerialMacOS.h)
endif()
include_directories(Transcievers)
PREPEND(UniversalCan_INC)
set(UniversalCan_INC ${UniversalCan_INC}  PARENT_SCOPE)