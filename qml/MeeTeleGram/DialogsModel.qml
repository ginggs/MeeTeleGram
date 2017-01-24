import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    default property alias model: dialogs_model

    property bool ready: false

    ListModel {
        id: dialogs_model
    }

    Connections {
        target: telegram
        onDialog_received: {
            console.log("Dialog received: name: " + dialog["name"])
            dialogs_model.append(dialog)
            ready = true
        }
    }
}
