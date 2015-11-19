import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    default property alias model: contacts_model

    property bool ready: false

    ListModel {
        id: contacts_model
    }

    Connections {
        target: telegram
        onContact_list_received: {
            console.log("Contact list received: num = " + contacts.length)
            contacts_model.clear()
            for (var i = 0; i < contacts.length; ++i)
                contacts_model.append({"name": contacts[i]})
            ready = true
        }
    }
}
