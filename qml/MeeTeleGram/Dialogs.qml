import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0

Page {
    id: dialogs_page

    DialogsModel {
        id: dialogs_model
    }

    BusyIndicator {
        id: spinner
        platformStyle: BusyIndicatorStyle { size: "large" }
        anchors.centerIn: parent
        running: !dialogs_model.ready
        visible: running
    }

    Component {
        id: dialog_delegate

        Item {
            id: listItem
            width: parent.width
            height: 80

            BorderImage {
                id: background

                anchors {
                    fill: parent
                    leftMargin: -dialogs_page.anchors.leftMargin
                    rightMargin: -dialogs_page.anchors.rightMargin
                }

                visible: mouseArea.pressed
                source: "image://theme/meegotouch-list-inverted-background-pressed-center"
            }

            Image {
                id: contactImage
                height: 60
                width: 60
                source: "image://theme/meegotouch-avatar-placeholder-background"

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: UiConstants.DefaultMargin
                }

                smooth: true

                Image {
                    id: contactPic
                    anchors.fill: parent
                    source: avatar ? avatar : ""

                    onStatusChanged: if (status === Image.Error)
                                         contactPic.source = ""
                }

                Label {
                    id: tagName
                    anchors.centerIn: parent
                    color: "white"
                    font { bold: true; pixelSize: 32 }
                    opacity: (contactPic.source == "") ? 1.0 : 0.0
                    text: name[0].toUpperCase()
                }
            }

            Image {
                id: maskedImage
                height: 60
                width: 60
                source: "image://theme/meegotouch-avatar-mask-small"

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: UiConstants.DefaultMargin
                }

                smooth: true
                opacity: 0.0
            }

            Label {
                id: contactName
                width: parent.width - (unread_bubble.width + contactPic.width + UiConstants.DefaultMargin + 50)
                anchors {
                    left: contactImage.right
                    leftMargin: 20
                    top: parent.top
                    topMargin: UiConstants.DefaultMargin
                    rightMargin: UiConstants.DefaultMargin
                }

                font: UiConstants.TitleFont
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                color: "black"
                text: name
                maximumLineCount: 1
            }

            CountBubble {
                id: unread_bubble
                anchors {
                    left: contactName.right
                    top: parent.top
                }

                largeSized: true
                value: unread_count
                visible: unread_count
            }

            Label {
                id: lastMsg
                width: parent.width - (contactPic.width + UiConstants.DefaultMargin + 20)
                anchors {
                    left: contactImage.right
                    leftMargin: 20
                    top: contactName.bottom
                    rightMargin: UiConstants.DefaultMargin
                }

                font.pixelSize: 16
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                text: message ? message : ""
                maximumLineCount: 1
            }

            Label {
                id: msgDate
                width: parent.width - (contactPic.width + UiConstants.DefaultMargin + 20)
                anchors {
                    left: contactImage.right
                    leftMargin: 20
                    top: lastMsg.bottom
                    rightMargin: UiConstants.DefaultMargin
                }

                font.pixelSize: 10
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                text: message_date
                maximumLineCount: 1
            }

            MouseArea {
                id: mouseArea
                anchors.fill: background
                onClicked: {
                    console.log("Dialog clicked: " + name + peer_id.type)
                    chat_window.open()
                    chat_window.load(peer_id)
                }
            }
        }
    }

    ListView {
        id: dialogs_list
        anchors.fill: parent
        model: dialogs_model.model
        delegate: dialog_delegate
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

    ScrollDecorator { flickableItem: dialogs_list }
    FastScroll {listView: dialogs_list; alignment: Qt.AlignRight }
}

