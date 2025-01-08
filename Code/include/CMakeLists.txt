set(UniversalCan_INC
    CanMessage.h
    CanMessageTemplate.h
    ProtocolMapper.h
    Transcievers/ELM327.h
    Transcievers/ISerial.h
    Transcievers/ITransciever.h
    Transcievers/SerialMacOS.h
    )
include_directories(Transcievers)
PREPEND(UniversalCan_INC)
set(UniversalCan_INC ${UniversalCan_INC}  PARENT_SCOPE)