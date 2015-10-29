import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
    id: mainwindow

    initialPage: mainPage

    MainPage {
        id: mainPage
    }

    Component.onCompleted: {
        console.log("HIHIHIHI");
        telegram.login();
    }
    Connections {
        target: telegram
        onLogged_in: console.log("Logged in!")
        onContact_list_received: console.log("Contact list received: num = " + size)
        onPhone_number_requested: console.log("ask phone num")
        onCode_requested: console.log("ask code")
        onCurrent_pass_requested: console.log("ask curent pass")
        onRegister_info_requested: console.log("ask register info")
        onNew_password_requested: console.log("ask new pass")
        onCur_and_new_password_requested: console.log("as cur & new pass")
    }
}
