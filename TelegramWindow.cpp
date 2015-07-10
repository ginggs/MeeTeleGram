#include "TelegramWindow.h"
#include "ui_TelegramWindow.h"

#define TELEGRAM_N9_APP_HASH "923618563c7c9e07496c4aebb6924bfb"
#define TELEGRAM_N9_APP_ID 44097
#define TELEGRAM_N9_VERSION "1.0.0"


TelegramWindow::TelegramWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::TelegramWindow), qtlg(TELEGRAM_N9_APP_ID,
            TELEGRAM_N9_APP_HASH, "Telegram-N9 " TELEGRAM_N9_VERSION, ".",
            "/etc/telegram-n9/server.pub")
{
    ui->setupUi(this);

    qtlg.login();
}

TelegramWindow::~TelegramWindow()
{
    delete ui;
}

void TelegramWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}
