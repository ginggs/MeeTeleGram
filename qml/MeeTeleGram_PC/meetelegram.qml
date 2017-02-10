import QtQuick 1.1

Rectangle {
    id: mainwindow
    anchors.fill: parent

    Component.onCompleted: {
        telegram.login();
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
            console.log("start registration...")
            telegram.set_phone_number(
                        telegram.get_str_from_console("Please enter your phone number: "))
        }
        onCode_requested: {
            console.log("ask code")
            telegram.set_code(telegram.get_str_from_console("Enter login code: "))
        }
        onCurrent_pass_requested: console.log("ask curent pass")
        onRegister_info_requested: {
            console.log("ask register info")
            telegram.set_register_info("H", "V")
        }
        onNew_password_requested: console.log("ask new pass")
        onCur_and_new_password_requested: console.log("as cur & new pass")

        /////////////////////////////////////////////
        onContact_list_received: {
            console.log("Contact list received: num = " + contacts.length)
            for (var i = 0; i < contacts.length; ++i)
                console.log("Contact: " + contacts[i])
        }
        onDialog_received: {
            console.log("Dialog received: " + dialog)
        }
        onNew_message: {
            console.log("DIALOG ID: " + message["dialog_id"])
            if (!message["dialog_id"])
                return;
            console.log("New Message received: " + message)
        }

//        telegram.load_messages(peer_id, 0, 50, false)
        onMessage_received: {
            console.log("Old Message received: " + message)
        }

    }
}
