import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
    id: mainwindow

    initialPage: main_page

    MainPage {
        id: main_page
    }

    Component.onCompleted: {
        telegram.login();
    }

    Register {
        id: register_sheet
        onAccepted: {
            console.log("start registration...")
            telegram.set_phone_number(register_sheet.reginfo.phone_number)
        }
        onRejected: Qt.quit()
    }

    CodeRequest {
        id: code_request
        onAccepted: {
            telegram.set_code(code)
            console.log("Code entered: " + code)
        }
        onRejected: Qt.quit()
    }

    Chat {
        id: chat_window
        onAccepted: {
            console.log("Close chat window")
        }
    }

    Connections {
        target: telegram
        onLogged_in: {
            console.log("Logged in!")
            telegram.request_contact_list()
        }
        onStarted: {
            telegram.get_dialog_list()
        }

        onPhone_number_requested: {
            console.log("ask phone num")
            register_sheet.open()
        }
        onCode_requested: {
            console.log("ask code")
            code_request.open()
        }
        onCurrent_pass_requested: console.log("ask curent pass")
        onRegister_info_requested: {
            console.log("ask register info")
            telegram.set_register_info(register_sheet.reginfo.first_name,
                                       register_sheet.reginfo.last_name)
        }
        onNew_password_requested: console.log("ask new pass")
        onCur_and_new_password_requested: console.log("as cur & new pass")
    }
}
