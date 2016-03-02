#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcustomplot.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // Эта функция отвечает за построение
    // и обновления графика, с каждым тиком таймера
    void updatePlot();
    void on_intervalbutton_clicked();
    void on_stopbutton_clicked();
private:
    QTimer *timer;
    Ui::MainWindow *ui;
    //наш виджет
    QCustomPlot * m_CustomPlot;
    // это объект, который будет содержать текущее значение в качестве текста
    // который появится справа скраю
    QCPItemText *m_ValueIndex;

    QLineEdit *NEWedit;//для ввода интервала
    QPushButton *intervalbutton;//кнопка для установки интервала
    QPushButton *stopbutton;//кнопка для установки интервала
    // время между каждым обновлением
    // будет использоваться при вызове слота "updatePlot"
    float timeInterval;

    float intervalola;

    // буферы данных
    QVector<qreal> m_YData;
    QVector<qreal> m_XData;
};

#endif // MAINWINDOW_H
