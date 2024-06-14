#ifndef ADET_H
#define ADET_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QLine>
#include <QFrame>
#include <QProgressBar>
#include <QRegularExpression>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QSqlError>
#include <QMap>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class AdeT;
}
QT_END_NAMESPACE

struct tarea {
    int id, tipo, porciento;
    QString texto;
    bool estado;
};

class AdeT : public QMainWindow
{
    Q_OBJECT

public:
    AdeT(QWidget *parent = nullptr);
    ~AdeT();

private slots:
    void on_agregar_clicked();
    void obtenertareas();
    void eliminarHijos();
    void crearBaseDatos(QString acttema);
    void eliminarHijosRecursivo(QLayout* layout);
    void eliminartareas(const int& id);
    void modificartarea(int id, bool estado=false, int porciento=0,int modificar=0);
    void comboboxcomprovar(bool op,QLabel *text);
    void obtenertablenombres();
    void agregartarea(tarea tar,bool guardar=false);
    void on_tipo_currentTextChanged(const QString &arg1);
    void on_tarea_textChanged(const QString &arg1);
    void on_pushButton_clicked();
    void on_crear_clicked();
    void on_tem_currentTextChanged(const QString &arg1);
    void on_del_clicked();

private:
    Ui::AdeT *ui;
    bool insertar(tarea tar);
    QMap<QHBoxLayout*, int> idtaraa;
    QString tema = "tareas";
    int id=0;

};
#endif // ADET_H
