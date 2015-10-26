import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
    id: mainwindow

    Component.onCompleted: {
        console.log("HIHIHIHI");
        telegram.login();
    }
}
