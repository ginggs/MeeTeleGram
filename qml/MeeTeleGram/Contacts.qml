import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: contacts_page

    property bool ready: false

    BusyIndicator {
        id: spinner
        platformStyle: BusyIndicatorStyle { size: "large" }
        anchors.centerIn: parent
        running: !ready
        visible: running
    }

    Component {
        id: contact_msg_delegate

        Item {
            id: listItem
            width: parent.width
            height: 80

            BorderImage {
                id: background

                anchors {
                    fill: parent
                    leftMargin: -contacts_page.anchors.leftMargin
                    rightMargin: -contacts_page.anchors.rightMargin
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
                    source: profile_pic ? profile_pic : ""

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
                width: parent.width - (contactPic.width + UiConstants.DefaultMargin + 20)
                anchors {
                    left: contactImage.right
                    leftMargin: 20
                    verticalCenter: parent.verticalCenter
                    rightMargin: UiConstants.DefaultMargin
                }

                font: UiConstants.TitleFont
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                color: "black"
                text: name
                maximumLineCount: 1
            }

            MouseArea {
                id: mouseArea
                anchors.fill: background
                onClicked: {
                    console.log("Contact clicked: " + name)
                }
            }
        }
    }

    ListView {
        id: contact_list
        anchors.fill: parent
        model: telegram.get_contacts_model()
        delegate: contact_msg_delegate
        section.property: "contact_group"
        section.criteria: ViewSection.FullString
        section.delegate: Item {
            id: textblock
            width: contact_list.width
            height: 20

            Image {
                anchors {
                    right: text.left
                    left: parent.left
                    verticalCenter: text.verticalCenter
                    rightMargin: 24
                }

                source: "image://theme/meegotouch-groupheader"
                        + (theme.inverted ? "-inverted" : "") + "-background"
            }

            Label {
                id: text
                anchors {
                    verticalCenter: parent.verticalCenter
                    top: parent.top
                    right: textblock.right
                    rightMargin: 30
                }

                text: section
                color: "#8c8c8c"
                font: UiConstants.GroupHeaderFont
            }
        }
    }

    Connections {
        target: telegram
        onContact_list_received: {
            console.log("UI: Contact list received")
            ready = true
        }
    }

    FastScroll {
        listView: contact_list
        alignment: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.AlignLeft : Qt.AlignRight
    }
}

