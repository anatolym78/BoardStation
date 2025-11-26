/**
 * @file messagelistener.cpp
 * @brief Определение интерфейса MessageListener
 */
#include <QDebug>

#include "messagelistener.h"
#include "subjectcontainer.h"

static const std::string k_ModuleName = "MessageListener";

void MessageListener::update(const Subject &subject) {
    ConstMessage message;

    if (auto msg = dynamic_cast<const SubjectContainer<Message> *>(&subject)) {
        message = static_cast<const Message &>(*msg);
    }
    else if (auto constMsg =
                dynamic_cast<const SubjectContainer<ConstMessage> *>(
                    &subject)) {

        message = *constMsg;
    }


    if (!message) {
        qDebug() << QString::fromStdString(k_ModuleName) + "::update: Unexpected subject type";
    }


    messageReceived(std::move(message));
}
