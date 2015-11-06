import QtQuick 1.1
import com.nokia.meego 1.0

Sheet {
    id: registerSheet

    property variant reginfo: { "first_name": first_name.text,
                            "last_name": last_name.text,
                            "phone_number": phone_number.text }

    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    acceptButtonText: qsTr("Continue")
    rejectButtonText: qsTr("Cancel")

    content: /*Item {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.topMargin: 10*/
//        contentWidth: col2.width
//        contentHeight: col2.height
//        flickableDirection: Flickable.VerticalFlick
        Column {
            id: col2
            anchors.fill: parent
//            width: 480
            spacing: 10

            Row {
                width: parent.width
                spacing: 10
                Text {
                    text: qsTr("First Name:")
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
                TextField {
                    id: first_name
                    width: parent.width - parent.children[0].width - 20 - parent.spacing
                }
            }
            Row {
                width: parent.width
                spacing: 10
                Text {
                    text: qsTr("Last Name:")
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
                TextField {
                    id: last_name
                    width: parent.width - parent.children[0].width - 20 - parent.spacing
                }
            }
            Row {
                width: parent.width
                spacing: 10
                Text {
                    text: qsTr("Phone Number:")
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
                TextField {
                    id: phone_number
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    width: parent.width - parent.children[0].width - 20 - parent.spacing
                }
            }
//        }
    }

    onStatusChanged: {
        if (status === DialogStatus.Opening)
        {
            first_name.text = reginfo.first_name
            last_name.text = reginfo.last_name
            phone_number.text = reginfo.phone_number
        }
    }

    onAccepted: {
        reginfo = { "first_name": first_name.text,
            "last_name": last_name.text,
            "phone_number": phone_number.text }
    }
}
