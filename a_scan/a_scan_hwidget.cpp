/**
 * @file a_scan_hwidget.cpp
 * @brief A扫横向显示
 * @author Jake Yang <yanghuanjie@cndoppler.cn>
 * @date 2017-06-28
 */
#include "a_scan_hwidget.h"
#include <QDebug>
#include <QPainter>
#include <QThread>
#include <QTime>

static int flag = 0;
struct timeval start_tv;
struct timeval end_tv;

AscanHWidget::AscanHWidget(QWidget *parent) :
    AscanWidget(parent)
{
    setFixedSize(800, 800);
    setWindowTitle("ASCAN");
    setAutoFillBackground(true);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0, 0, 0));
    setPalette(palette);

	qDebug ("width = %d, height = %d ", width(), height());

    m_draw_thread = new MyThread;
    connect(m_draw_thread, SIGNAL(draw_data(QByteArray)),
            this, SLOT(recive_data(QByteArray)), Qt::QueuedConnection);
    m_draw_thread->set_flag(true);
    m_draw_thread->start();

}

AscanHWidget::~AscanHWidget()
{
    m_draw_thread->set_flag(false);
    m_draw_thread->quit();
    m_draw_thread->wait();
    m_draw_thread->deleteLater();
}

int AscanHWidget::x_axis_length() const
{
    return width();
}

int AscanHWidget::y_axis_length() const
{
    return height();
}

void AscanHWidget::paintEvent(QPaintEvent *e)
{
//    qDebug("%s[%d]: ",__func__, __LINE__);

    QPainter painter(this);
    painter.translate(0, height());

    QTransform form = painter.transform();

    form.rotate(180, Qt::XAxis);

    painter.setTransform(form);

    painter.setPen( wave_color() );
    painter.drawPath( paint_wave() );
}


void AscanHWidget::recive_data(QByteArray waveData)
{
	if (0 == flag) {
		if (gettimeofday(&start_tv, NULL) == 0) {
			flag = 1;
		}
	} else {
		if (gettimeofday(&end_tv, NULL) == 0) {
			printf ("interval time : end - start = %d \n", end_tv.tv_usec - start_tv.tv_usec);
		}
		flag = 0;
	}
    this->AscanWidget::show(waveData);
}