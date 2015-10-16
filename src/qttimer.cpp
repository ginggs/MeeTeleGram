/*
 * qttimers.cpp
 *
 *  Created on: ۱۸ تیر ۱۳۹۴ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#include <QObject>
#include <tgl.h>

typedef void (*timer_callback)(tgl_state *tl_state, void *arg);

static tgl_timer *my_timer_alloc(tgl_state *tl_state, timer_callback cb,
    void *arg);
static void my_timer_insert (tgl_timer *t, double p);
static void my_timer_delete (tgl_timer *t);
static void my_timer_free (tgl_timer *t);

tgl_timer_methods qtg_timers =
{
    my_timer_alloc,
    my_timer_insert,
    my_timer_delete,
    my_timer_free
};


struct tgl_timer: public QObject
{
    private:
        Q_OBJECT;

    public:
        tgl_timer(timer_callback cb, tgl_state *tls, void *cb_arg,
            QObject *parent = NULL) :
                QObject(parent), callback(cb), tl_state(tls), callback_arg(
                    cb_arg), id(0)
        {
        }

        void start(double p)
        {
            if (p < 0) { p = 0; }
            // if a timeout is schedeled before, it is replaced
            stop();
            id = startTimer(p * 1000);
        }

        void stop()
        {
            if (id)
            {
                killTimer(id);
                id = 0;
            }
        }

    protected:
        void timerEvent(QTimerEvent *event)
        {
            stop();
            callback(tl_state, callback_arg);
        }

    private:
        timer_callback callback;
        tgl_state *tl_state;
        void *callback_arg;
        int id;
};

static tgl_timer *my_timer_alloc(tgl_state *tl_state, timer_callback cb, void *arg)
{
    return new tgl_timer(cb, tl_state, arg);
}

static void my_timer_insert(tgl_timer *t, double p)
{
    t->start(p);
}

static void my_timer_delete(tgl_timer *t)
{
    t->stop();
}

static void my_timer_free(tgl_timer *t)
{
    t->deleteLater();
}

#include "qttimer.moc"
