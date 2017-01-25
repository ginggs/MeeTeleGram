import QtQuick 1.1
import com.nokia.meego 1.0

Sheet {
    id: code_input

    property int code

    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    acceptButtonText: qsTr("Register")
    rejectButtonText: qsTr("Cancel")

    content: /*Item {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.topMargin: 10*/
//        contentWidth: col2.width
//        contentHeight: col2.height
//        flickableDirection: Flickable.VerticalFlick
//        Column {
//            id: col2
////            width: 480
//            spacing: 10

            Row {
                anchors.fill: parent
//                width: parent.width
                spacing: 10
                Label {
                    id: code_label
                    text: qsTr("Code:")
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
                TextField {
                    id: input_code
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - parent.children[0].width - 20 - parent.spacing
                    anchors.left: code_label.right
                }
            }
//        }
//    }

    onAccepted: code = input_code.text
}
