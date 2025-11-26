/**
 * @file protocolfactory.h
 * @brief Объявление фабрики протоколов информационного взаимодействия
 */
#include "protocolfactory.h"

// #include "radioprotocol.h"
#include "serialprotocol.h"

// Protocol_Ptr ProtocolFactory::createRadioProtocol(Radio::Role role) {
//     RadioProtocol *protocol = new RadioProtocol(role);
//     return Protocol_Ptr(protocol);
// }

Protocol_Ptr ProtocolFactory::createSerialProtocol() {
    SerialProtocol *protocol = new SerialProtocol;
    return Protocol_Ptr(protocol);
}
