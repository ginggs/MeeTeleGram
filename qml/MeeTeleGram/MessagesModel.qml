import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    default property alias model: messages_model

    property bool ready: false

    function add_new_message(msg)
    {
        messages_model.append(msg)
    }

    ListModel {
        id: messages_model
    }

    Connections {
        target: telegram
        onMessage_received: {
            messages_model.insert(0, message)
        }
    }
}
