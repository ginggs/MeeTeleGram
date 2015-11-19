import QtQuick 1.1
import com.nokia.meego 1.0


Page {
    signal showAbout

    function set_contacts(contacts)
    {
        contacts_model.clear()
        for (var i = 0; i < contacts.length; ++i)
            contacts_model.append({"name": contacts[i]})
    }

    tools: commonTools
    anchors.margins: 10

    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    onShowAbout: aboutDialog.open()

    Component.onCompleted:
    {
        // sample signal connection: mainPage.addLocation.connect(locationWindow.addLocation)
    }

    ListModel {
        id: contacts_model
    }

    Component {
        id: contact_msg_delegate
        Item {
            width: parent.width; height: 80
            Column {
                Text { text: '<b>' + name  + '</b>'}
            }
        }
    }

    ListView {
        anchors.fill: parent
        model: contacts_model
        delegate: contact_msg_delegate
    }

    ToolBarLayout {
        id: commonTools
        visible: true
        ToolIcon {
            iconId: "toolbar-view-menu"
//            anchors.right: (parent === undefined) ? undefined : parent.right
            onClicked: (myMenu.status === DialogStatus.Closed) ? myMenu.open()
                                                               : myMenu.close()
        }
//        ToolIcon {
//            iconId: "toolbar-grid"
////            platformIconId: "toolbar-grid"
////            anchors.right: menutool.left
////            onClicked: (myMenu.status === DialogStatus.Closed) ? myMenu.open() : myMenu.close()
//        }
//        ToolIcon {
//            iconId: "toolbar-list"
//        }
    }

    QueryDialog {
        id: aboutDialog

        icon: "/opt/MeeTeleGram/icons/MeeTeleGram.png"
        titleText: qsTr("About MeeTeleGram 0.0")
        message: qsTr("Copyright (C) 2015 <br/><br/>    \
            <b>Hedayat Vatankhah</b>  <br/><br/>        \
            License: GNU GPLv3+<br/><br/>               \
            This program comes with ABSOLUTELY NO WARRANTY. \
            This is free software, and you are welcome to redistribute it   \
            under terms of GNU GPL version 3 (or later). <br/>  \
                http://www.gnu.org/licenses/")
        acceptButtonText: "OK"
    }

    Menu {
        id: myMenu
        visualParent: pageStack
        MenuLayout {
/*            MenuItem { text: qsTr("Locations")
                onClicked: {
                    locationWindow.selectionMode = false;
                    pageStack.push(locationWindow)
                }
            }*/
            MenuItem { text: qsTr("About")
                onClicked: {
                    aboutDialog.open()
                }
            }
        }
    }

}
