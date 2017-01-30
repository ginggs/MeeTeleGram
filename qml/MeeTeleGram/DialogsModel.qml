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
//            console.log("Dialog received: name: " + dialog["name"])
            dialogs_model.append(dialog)
            ready = true
        }
        onNew_message: {
            console.log("DIALOG ID: " + message["dialog_id"])
            if (!message["dialog_id"])
                return;
            console.log("Finding corresponding dialog...")
            for (var i=0; i<dialogs_model.count; i++)
            {
                if (dialogs_model.get(i)["peer_id"].is_equal(message["dialog_id"]))
                {
                    console.log("Found at: " + i)
                    dialogs_model.move(i, 0, 1)
                    dialogs_model.setProperty(0, "message", message["message"])
                    if (message["unread"])
                        dialogs_model.setProperty(0, "unread",
                                                  dialogs_model.get(0)["unread"]+1)
                }
            }
        }
    }
}
