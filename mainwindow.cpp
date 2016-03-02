
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_CustomPlot( 0 )
{
    timeInterval = 5000;

    setMinimumWidth( 900 );
    // Выделение памяти для объекта   виджета
    m_CustomPlot = new QCustomPlot( this );
    // Следующая строка отвечает за добавление
    //  виджета в главное окно  приложения

    ui->setupUi(this);

    // добавляем график на виджет (основной график)
    m_CustomPlot->addGraph();
    // это будет горизонтальная линия
    // которая наследует текущее значение основного графика
    m_CustomPlot->addGraph();

    //отображеие оси иксов как времени
    // вместо цифр
    m_CustomPlot->xAxis->setTickLabelType( QCPAxis::ltDateTime );
    // установка формата времени
    m_CustomPlot->xAxis->setDateTimeFormat( "hh:mm:ss:zzz" );
    m_CustomPlot->xAxis->setAutoTickStep( true ); //aвтоматическое масштабирование тиков по Оси X
    m_CustomPlot->yAxis->setAutoTickStep( true );

    // выделение памяти для текста, который будет
    // отражать текущее значение как текст
    m_ValueIndex = new QCPItemText( m_CustomPlot );
    // установка шрифта
    m_ValueIndex->setFont(QFont(font().family(), 10));
    //добавление текста к графику
    m_CustomPlot->addItem( m_ValueIndex );

    // установка цветов графика
    QColor brushClr = Qt::green;
    brushClr.setAlphaF( .5 );
    m_CustomPlot->graph( 0 )->setBrush( brushClr );
    m_CustomPlot->graph( 0 )->setPen( QColor() );
    m_CustomPlot->graph( 1 )->setPen( QColor() );

    // установка и запуск таймера
    timer = new QTimer(this);
    connect(timer, SIGNAL( timeout() ), this, SLOT( updatePlot() ) );
    timer->start( timeInterval );

    setCentralWidget( m_CustomPlot );
    //добавила окошко для интервала
    NEWedit = new QLineEdit ( this );
    NEWedit->setGeometry(40,20,60,30);
    NEWedit->show();

    // добавляю кнопку установки интервала введенного(start)
    intervalbutton = new QPushButton ( this );
    intervalbutton->setGeometry(100,20,60,30);
    intervalbutton->show();
    intervalbutton->setText("старт");

    // добавляю кнопку stop
    stopbutton = new QPushButton ( this );
    stopbutton->setGeometry(200,20,60,30);
    stopbutton->show();
    stopbutton->setText("стоп");

    connect(intervalbutton, SIGNAL(clicked(bool)), this, SLOT(on_intervalbutton_clicked()));
    connect(stopbutton, SIGNAL(clicked(bool)), this, SLOT(on_stopbutton_clicked()));
}
void MainWindow::on_intervalbutton_clicked()
{
    QString intervalio = NEWedit->text(); //текст берем из окошка

    bool ok; //для проверки
    float intervalola = intervalio.toFloat ( &ok ); // преобразовываем  в число
    if (!ok)
    {
        QMessageBox::information(this,"Ошибочка", "Введите вещественное число");
        return;
    }

    timeInterval = intervalola;
    qDebug() << timeInterval;
    timer->stop();
    timer->start( timeInterval );
     statusBar()->showMessage("таймер запущен");
}

//стопчик
void MainWindow::on_stopbutton_clicked()
{

    timeInterval = intervalola;
    qDebug() << timeInterval;
    timer->stop();
   //timer->start( timeInterval );
    statusBar()->showMessage("таймер остановлен");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updatePlot()
{

    // берем текущее время
    QTime tm = QTime::currentTime();
    // переводим его в милисекунды
    qreal seconds = 60 * 60 * tm.hour() + 60 * tm.minute() + tm.second();
    qreal timeValue = 1000 * seconds + tm.msec();
    // добавляем время в буфер
    m_XData.append( timeValue );
    // генерируем случайные данные
    // здесь создаем случайное целое число между [ 0 , 1 ]
    qreal r = static_cast<qreal>( rand() ) / RAND_MAX  ;
    // следующее значеие будет таким:
    qreal value = 80 + 5 * r;
    m_YData.append( value );

    // ставим размер буфера до 100 для каждого,
    // чтобы не было переполнения случайными числами
    if( m_XData.size() > 100 ){
        m_XData.remove( 0 );
        m_YData.remove( 0 );
    }

    // Добавляем данные на график
    m_CustomPlot->graph( 0 )->setData( m_XData , m_YData );
    // теперь самое сложное

    // устанавливаем диапазон вертикальной и горизонтальной оси участка (не относится к графику  )
    // так, что все данные будут отцентрованы, сначала мы получаем минимальные и максимальные из х и у данных
    QVector<double>::iterator xMaxIt = std::max_element( m_XData.begin() , m_XData.end() );
    QVector<double>::iterator xMinIt = std::min_element( m_XData.begin() , m_XData.end() );
    QVector<double>::iterator yMaxIt = std::max_element( m_YData.begin() , m_YData.end() );


    qreal yPlotMin = 0;
    qreal yPlotMax = *yMaxIt;

    qreal xPlotMin = *xMinIt;
    qreal xPlotMax = *xMaxIt;

    // это для уверенности что график не будет занимать все пространство виджета
    // делаются отступы
    qreal yOffset = 0.3 * ( yPlotMax - yPlotMin ) ;
    qreal xOffset = 0.5 *( xPlotMax - xPlotMin );
    m_CustomPlot->xAxis->setRange( xPlotMin , xPlotMax + xOffset );
    m_CustomPlot->yAxis->setRange(yPlotMin , yPlotMax + yOffset);
    //************************************************************//
    // создаем данные для горизонтальной линии, которая меняется
    // с последним значением главного графика
    QVector<double> tmpYYData;
    QVector<double> tmpXXData;
    // так как это горизонтальная линия, нам надо определить ее двумя линиями
    // и эти две линии имеют y значение
    tmpYYData.append( m_YData.last() );
    tmpYYData.append( m_YData.last() );

    // чтобы создать линию пересекающую виджет горизонтально
    // с левого края до правого

    tmpXXData.append( m_XData.first() );
    tmpXXData.append( m_XData.last() + xOffset );
    // наконец обозначаем данные горизонатльной линии
    m_CustomPlot->graph( 1 )->setData( tmpXXData , tmpYYData );
    //************************************************************//
    //теперь текстовое примечание, которое отображает текущее значение
    //в виде строки, это координаты текстового элемента, перемещения(смещения) тут
    // только для того чтобы  текст появился за и выше горизонтальной линии.

    qreal indexX = m_XData.last() + 0.5 * xOffset;
    qreal indexY = m_YData.last() + 0.2 * yOffset;

    // устанавливаем посчитанные координаты
    m_ValueIndex->position->setCoords( indexX , indexY );
    // устанавливаем текст, который хотим отобразить
    m_ValueIndex->setText(  QString::number( tmpYYData.last() ) + "  ВЕРТОЛЕТИКОВ" );


    // обновляем виджет
    m_CustomPlot->replot();
}
