#ifndef TELEGRAMWINDOW_H
#define TELEGRAMWINDOW_H

#include <QMainWindow>
#include "qtelegram.h"

namespace Ui {
class TelegramWindow;
}

class TelegramWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit TelegramWindow(QWidget *parent = 0);
        ~TelegramWindow();

    protected:
        void changeEvent(QEvent *e);

    private:
        Ui::TelegramWindow *ui;
        qtelegram qtlg;
};

#endif // TELEGRAMWINDOW_H
