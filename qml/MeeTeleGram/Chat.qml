import QtQuick 1.1
import com.nokia.meego 1.0

Sheet {
    id: chat_sheet

    acceptButtonText: qsTr("Chats")
    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    QtObject {
        id: internal
        property variant peer
    }

    function load(peer_id) {
        messages_model.model.clear()
        telegram.load_messages(peer_id, 0, 50, false)
        messages_list.positionViewAtEnd()
        internal.peer = peer_id
    }

    onAccepted: {
        messages_model.model.clear()
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
                color: telegram.is_our_id(from_id) ? "blue" : "white"
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
                    color: telegram.is_our_id(from_id) ? "white" : "black"
                    text: message
                }
            }


            MouseArea {
                id: mouseArea
                anchors.fill: background
                onClicked: {
                    console.log("Message clicked")
                }
            }
        }
    }

    content: Item {
        id: chat_column
        anchors {
            fill: parent
        }
        ListView {
            id: messages_list
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: my_message.top
                leftMargin: UiConstants.DefaultMargin * 2
                rightMargin: UiConstants.DefaultMargin * 2
            }
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
        ScrollDecorator { flickableItem: messages_list }

        TextArea {
            id: my_message
            height: 60
            anchors {
                left: parent.left
                right: parent.right
                bottom: input_button_holder.top
            }
        }

        Rectangle {
            id: input_button_holder
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            width: parent.width
            height: my_message.activeFocus ? 72 : 0
            color: theme.inverted? "#1A1A1A" : "white"
            clip: true

            MouseArea {
                id: input_button_holder_area
                anchors.fill: parent
                onClicked: {
//                    sendMediaWindow.opacity = 0
//                    showSendButton=true;
//                    forceFocusToChatText()
                }
            }


            Rectangle {
                height: 1
                width: parent.width
                x:0; y:0
                color: "gray"
                opacity: 0.4
            }

//            Button
//            {
//                id: emoji_button
//                //platformStyle: ButtonStyle { inverted: true }
//                width:50
//                height:50
//                iconSource: "../common/images/emoji/32/E415.png"
//                anchors.left: parent.left
//                anchors.leftMargin: 16
//                anchors.verticalCenter: send_button.verticalCenter
//                onClicked: {
//                    sendMediaWindow.opacity = 0
//                    emojiDialog.openDialog(chat_text);
//                    showSendButton=true;
//                    //chat_text.lastPosition = chat_text.cursorPosition
//                }
//            }

//            Button {
//                id: media_button
//                anchors.left: emoji_button.right
//                anchors.leftMargin: 12
//                anchors.verticalCenter: parent.verticalCenter
//                width: 50
//                height: width
//                iconSource: theme.inverted ? "../common/images/attachment-white.png" : "../common/images/attachment.png"

//                onClicked: {
//                    if (sendMediaWindow.visible) sendMediaWindow.opacity = 0
//                    else sendMediaWindow.opacity = 1
//                    showSendButton=true;
//                    forceFocusToChatText()
//                }
//            }

            Button
            {
                id:send_button
                platformStyle: ButtonStyle { inverted: true }
                width:160
                height:50
                text: qsTr("Send")
                anchors.right: parent.right
                anchors.rightMargin: 16
                y: 10
                onClicked: {
                    if (my_message.text.length > 0 ) {
                        console.log("Sending message: " + my_message.text)
                        telegram.send_msg(internal.peer, my_message.text)
                        my_message.text = ""
                    } else
                        console.log("Not sending empty message")
                }
            }

        }

    }

//    FastScroll {
//        listView: contact_list
//        alignment: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.AlignLeft : Qt.AlignRight
//    }
}
