#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_canvas.h>
#include "PieMarker.h"
#include "PiePlot.h"

//class TimeScaleDraw: public QwtScaleDraw
//{
//public:
//    TimeScaleDraw(const QTime &base):
//        baseTime(base)
//    {
//    }
//    virtual QwtText label(double v) const
//    {
//        QTime upTime = baseTime.addSecs((int)v);
//        return upTime.toString();
//    }
//private:
//    QTime baseTime;
//};

//class Background: public QwtPlotItem
//{
//public:
//    Background()
//    {
//        setZ(0.0);
//    }

//    virtual int rtti() const
//    {
//        return QwtPlotItem::Rtti_PlotUserItem;
//    }

//    virtual void draw(QPainter *painter,
//        const QwtScaleMap &, const QwtScaleMap &yMap,
//        const QRectF &rect) const
//    {
//        QColor c(Qt::white);
//        QRectF r = rect;

//        for ( int i = 100; i > 0; i -= 10 )
//        {
//            r.setBottom(yMap.transform(i - 10));
//            r.setTop(yMap.transform(i));
//            painter->fillRect(r, c);

//            c = c.dark(110);
//        }
//    }
//};

class PiePlotCurve: public QwtPlotCurve
{
public:
    PiePlotCurve(const QString &title):
        QwtPlotCurve(title)
    {
        setRenderHint(QwtPlotItem::RenderAntialiased);
    }

    void setColor(const QColor &color)
    {
        QColor c = color;
        c.setAlpha(150);

        setPen(c);
        setBrush(c);
    }
};

PiePlot::PiePlot(QWidget *parent):
    QwtPlot(parent),
    dataCount(0)
{
    //setAutoReplot(false);

    canvas()->setBorderRadius( 10 );

    plotLayout()->setAlignCanvasToScales(true);

    QwtLegend *legend = new QwtLegend;
    legend->setItemMode(QwtLegend::ReadOnlyItem);
    insertLegend(legend, QwtPlot::RightLegend);

    //    setAxisTitle(QwtPlot::xBottom, " System Uptime [h:m:s]");
    //    setAxisScaleDraw(QwtPlot::xBottom,
    //        new TimeScaleDraw(PieStat.upTime()));
    //    setAxisScale(QwtPlot::xBottom, 0, HISTORY);
    //    setAxisLabelRotation(QwtPlot::xBottom, -50.0);
    //    setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

    /*
     In situations, when there is a label at the most right position of the
     scale, additional space is needed to display the overlapping part
     of the label would be taken by reducing the width of scale and canvas.
     To avoid this "jumping canvas" effect, we add a permanent margin.
     We don't need to do the same for the left border, because there
     is enough space for the overlapping label below the left scale.
     */

    //    QwtScaleWidget *scaleWidget = axisWidget(QwtPlot::xBottom);
    //    const int fmh = QFontMetrics(scaleWidget->font()).height();
    //    scaleWidget->setMinBorderDist(0, fmh / 2);

    //    setAxisTitle(QwtPlot::yLeft, "Pie Usage [%]");
    //    setAxisScale(QwtPlot::yLeft, 0, 100);

    enableAxis(QwtPlot::yLeft,false);
    enableAxis(QwtPlot::xBottom,false);
    PieMarker *pie = new PieMarker();
    pie->attach(this);
    
    PiePlotCurve *curve;

    curve = new PiePlotCurve("Plot 1");
    curve->setColor(Qt::green);
    curve->attach(this);
    data[Plot1].curve = curve;

    curve = new PiePlotCurve("Plot 2");
    curve->setColor(Qt::red);
    curve->setZ(curve->z() - 1);
    curve->attach(this);
    data[Plot2].curve = curve;

    curve = new PiePlotCurve("Plot 3");
    curve->setColor(Qt::blue);
    curve->setZ(curve->z() - 2);
    curve->attach(this);
    data[Plot3].curve = curve;

    curve = new PiePlotCurve("Plot 4");
    curve->setColor(Qt::cyan);
    curve->setZ(curve->z() - 3);
    curve->attach(this);
    data[Plot4].curve = curve;


    showCurve(data[Plot1].curve, false);
    showCurve(data[Plot2].curve, false);
    showCurve(data[Plot3].curve, false);
    showCurve(data[Plot4].curve, false);



    //    for ( int i = 0; i < HISTORY; i++ )
    //        timeData[HISTORY - 1 - i] = i;

    //    (void)startTimer(1000); // 1 second

    //    connect(this, SIGNAL(legendChecked(QwtPlotItem *, bool)),
    //            SLOT(showCurve(QwtPlotItem *, bool)));
}

void PiePlot::plotStaticValues(QVector<double> *inputData)
{
    if(inputData->size() == NumPlots)
        for (int i = 0; i < inputData->size(); i++)
            data[i].data[0] = inputData->at(i);


    if ( dataCount < HISTORY )
        dataCount++;

    for ( int j = 0; j < HISTORY; j++ )
        timeData[j]++;

    for ( int c = 0; c < NumPlots; c++ )
    {
        data[c].curve->setRawSamples(
                    timeData, data[c].data, dataCount);
    }
    replot();
}

void PiePlot::timerEvent(QTimerEvent *)
{
}

void PiePlot::showCurve(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
    QWidget *w = legend()->find(item);
    if ( w && w->inherits("QwtLegendItem") )
        ((QwtLegendItem *)w)->setChecked(on);
    
    replot();
}

