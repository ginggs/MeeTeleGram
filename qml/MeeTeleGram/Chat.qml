import QtQuick 1.1
import com.nokia.meego 1.0

Sheet {
    id: chat_sheet

    acceptButtonText: qsTr("Chats")
    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    function load(peer_id) {
        messages_model.model.clear()
        telegram.load_messages(peer_id)
    }

    onAccepted: {
        messages_model.model.clear()
        console.log("CLEARED")
    }

    MessagesModel {
        id: messages_model
    }

    Component {
        id: message_delegate

        Item {
            id: listItem
            width: parent.width
            implicitHeight: message_rect.implicitHeight + 20

            BorderImage {
                id: background

                anchors {
                    fill: parent
                    leftMargin: -chat_sheet.anchors.leftMargin
                    rightMargin: -chat_sheet.anchors.rightMargin
                }

                visible: mouseArea.pressed
                source: "image://theme/meegotouch-list-inverted-background-pressed-center"
            }

            Rectangle {
                id: message_rect
                color: "blue"
                width: parent.width
                implicitHeight: message_label.implicitHeight + 20
                Label {
                    id: message_label
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: UiConstants.DefaultMargin
                        topMargin: UiConstants.DefaultMargin
                        rightMargin: UiConstants.DefaultMargin
                    }

    //                font: UiConstants.TitleFont
                    wrapMode: Text.Wrap
                    elide: Text.ElideNone
                    color: "black"
                    text: message
                }
            }


            MouseArea {
                id: mouseArea
                anchors.fill: background
                onClicked: {
                    console.log("Message clicked: " + message)
                }
            }
        }
    }

    content: Column {
        anchors {
            fill: parent
            leftMargin: UiConstants.DefaultMargin
            rightMargin: UiConstants.DefaultMargin
        }
        ListView {
            id: messages_list
            width: parent.width
            height: parent.height - my_message.height
            model: messages_model.model
            delegate: message_delegate
    //        section.property: "name"
    //        section.criteria: ViewSection.FullString
    //        section.delegate: Item {
    //            id: textblock
    //            width: contact_list.width
    //            height: 20

    //            Image {
    //                anchors {
    //                    right: text.left
    //                    left: parent.left
    //                    verticalCenter: text.verticalCenter
    //                    rightMargin: 24
    //                }

    //                source: "image://theme/meegotouch-groupheader"
    //                        + (theme.inverted ? "-inverted" : "") + "-background"
    //            }

    //            Label {
    //                id: text
    //                anchors {
    //                    verticalCenter: parent.verticalCenter
    //                    top: parent.top
    //                    right: textblock.right
    //                }

    //                text: section
    //                color: "#8c8c8c"
    //                font: UiConstants.GroupHeaderFont
    //            }
    //        }
        }

        TextField {
            id: my_message
            height: 60
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: UiConstants.DefaultMargin*2
                rightMargin: UiConstants.DefaultMargin*2
            }
        }
    }

    ScrollDecorator { flickableItem: messages_list }
//    FastScroll {
//        listView: contact_list
//        alignment: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.AlignLeft : Qt.AlignRight
//    }
}
