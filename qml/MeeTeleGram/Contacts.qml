import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: contacts_page

    ContactsModel {
        id: contacts_model
    }

    Component {
        id: contact_msg_delegate
        Item {
            width: parent.width; height: 80

            Column {
                anchors.verticalCenter: parent.verticalCenter
                anchors.fill: parent

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.Wrap
                    text: name
                    font.bold: true
                }
            }
        }
    }

    ListView {
        id: contact_list
        anchors.fill: parent
        model: contacts_model.model
        delegate: contact_msg_delegate
        section.property: "name"
        section.criteria: ViewSection.FirstCharacter
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
                }

                text: section
                color: "#8c8c8c"
                font: UiConstants.GroupHeaderFont
            }
        }
    }

    ScrollDecorator { flickableItem: contact_list }
    SectionScroller { listView: contact_list }
}

