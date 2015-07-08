#ifndef TELEGRAMWINDOW_H
#define TELEGRAMWINDOW_H

#include <QMainWindow>

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
};

#endif // TELEGRAMWINDOW_H
