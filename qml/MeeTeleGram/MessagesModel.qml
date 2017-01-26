import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    default property alias model: messages_model

    property bool ready: false

    ListModel {
        id: messages_model
    }

    Connections {
        target: telegram
        onMessage_received: {
            console.log("Message received: " + message["message"])
            messages_model.insert(0, message)
        }
    }
}
