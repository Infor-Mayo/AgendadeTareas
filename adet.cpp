#include "adet.h"
#include "ui_adet.h"


AdeT::AdeT(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdeT)
{
    ui->setupUi(this);
    ui->agregar->setVisible(false);
    ui->tema->setVisible(false);
    ui->crear->setVisible(false);
    crearBaseDatos(tema);
    obtenertareas();
    obtenertablenombres();
}

AdeT::~AdeT()
{
    delete ui;
}

void AdeT::comboboxcomprovar(bool op,QLabel *text){
    if (op){
        text->setText("<s>"+text->text()+"</s>");
    }else{
        static const QRegularExpression regex("<s>(.*?)</s>");
        QRegularExpressionMatchIterator i = regex.globalMatch(text->text());
        QString filteredText;
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            filteredText += match.captured(1);
        }
        text->setText(filteredText);
    }
}

void AdeT::agregartarea(tarea tar,bool guardar){
    QHBoxLayout *cont = new QHBoxLayout();
    QLabel *text = new QLabel(tar.texto);
    QPushButton *elimunar = new QPushButton("");
    QProgressBar *pro=NULL;
    QCheckBox *cumplida=NULL;
    QPushButton *sumar=NULL;
    QFrame *line=NULL;

    text->setAlignment(Qt::AlignCenter);
    elimunar->setMaximumWidth(40);
    if (tar.tipo==2){
        cumplida = new QCheckBox("");
        if (tar.estado==true){
            comboboxcomprovar(tar.estado,text);
        }
        cumplida->setChecked(tar.estado);
        cumplida->setMaximumWidth(40);
        cont->addWidget(cumplida);
        QObject::connect(cumplida, &QCheckBox::stateChanged, [=]() {
            comboboxcomprovar(cumplida->isChecked(),text);
            modificartarea(idtaraa[cont],cumplida->isChecked());
        });
    }else if (tar.tipo==0){
        text->setText("<b>"+text->text()+"</b>");
    }else if (tar.tipo==3){
        line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        cont->addWidget(line);
        cont->addWidget(elimunar);
    }else if (tar.tipo==4){
        sumar = new QPushButton("+");
        sumar->setMaximumWidth(15);
        sumar->setFlat(true);
        pro = new QProgressBar();
        if (tar.porciento>=100){
            pro->setValue(100);
            sumar->deleteLater();
            sumar=NULL;
            text->setText("<s>"+text->text()+"</s>");
        }else{
            pro->setValue(tar.porciento);
        }
        QObject::connect(sumar, &QPushButton::clicked, [=]() {
            int npor=pro->value()+25;
            pro->setValue(npor);
            modificartarea(idtaraa[cont],false,npor,1);
        });
        QObject::connect(pro, &QProgressBar::valueChanged, [=]() mutable {
            if (pro->value()>=100){
                sumar->deleteLater();
                sumar=NULL;
                text->setText("<s>"+text->text()+"</s>");
            }
        });
        pro->setMaximumWidth(40);
        cont->addWidget(sumar);
        cont->addWidget(pro);
    }
    if (tar.tipo!=3){
        cont->addWidget(text);
        cont->addWidget(elimunar);
    }
    QObject::connect(elimunar, &QPushButton::clicked, [=]() {
        eliminartareas(idtaraa[cont]);
        cont->deleteLater();
        text->deleteLater();
        elimunar->deleteLater();
        if(pro!=NULL){
            pro->deleteLater();
        }
        if(line!=NULL){
            line->deleteLater();
        }
        if(sumar!=NULL){
            sumar->deleteLater();
        }
        if(cumplida!=NULL){
            cumplida->deleteLater();
        }
    });
    idtaraa.insert(cont,tar.id);
    ui->contv->addLayout(cont);
    ui->tarea->setText("");
    if (guardar==true){
        insertar(tar);
    }
}

void AdeT::on_agregar_clicked()
{
    tarea tar;
    tar.texto=ui->tarea->text();
    tar.tipo=ui->tipo->currentIndex();
    tar.porciento=0;
    tar.estado=false;
    id++;
    tar.id =id;
    agregartarea(tar,true);
}

void AdeT::eliminarHijos() {
    QVBoxLayout* layout = ui->contv;
    if (layout!= nullptr) {
        while (layout->count() > 0) {
            QLayoutItem* item = layout->takeAt(0);
            if (item!= nullptr) {
                QWidget* widget = item->widget();
                if (widget!= nullptr) {
                    widget->deleteLater();
                } else {
                    // Si el item no es un widget, entonces es un layout
                    QLayout* subLayout = qobject_cast<QLayout*>(item->layout());
                    if (subLayout!= nullptr) {
                        eliminarHijosRecursivo(subLayout); // Llama a una función recursiva para eliminar widgets de layouts anidados
                        delete subLayout; // Elimina el layout después de eliminar sus widgets
                    }
                }
            }
        }
    }
}

// Función recursiva para eliminar widgets de layouts anidados
void AdeT::eliminarHijosRecursivo(QLayout* layout) {
    if (layout == nullptr) return;

    while (layout->count() > 0) {
        QLayoutItem* item = layout->takeAt(0);
        if (item!= nullptr) {
            QWidget* widget = item->widget();
            if (widget!= nullptr) {
                widget->deleteLater();
            } else {
                QLayout* subLayout = qobject_cast<QLayout*>(item->layout());
                if (subLayout!= nullptr) {
                    eliminarHijosRecursivo(subLayout); // Recursión para layouts anidados
                    delete subLayout; // Elimina el layout después de eliminar sus widgets
                }
            }
        }
    }
}


void AdeT::on_tipo_currentTextChanged(const QString &arg1)
{
    if (arg1=="Linea"){
        ui->tarea->setVisible(false);
        ui->agregar->setVisible(true);
    }else{
        ui->tarea->setVisible(true);
        if (ui->tarea->text()==""){
            ui->agregar->setVisible(false);
        }
    }
}


void AdeT::on_tarea_textChanged(const QString &arg1)
{
    if (arg1!=""){
        ui->agregar->setVisible(true);
    }else{
        ui->agregar->setVisible(false);
    }
}

void AdeT::crearBaseDatos(QString acttema) {
    // Define la ruta del archivo de la base de datos
    QString rutaBaseDeDatos = QDir::currentPath() + "/tareas.db";

    // Crea o abre la conexión a la base de datos
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(rutaBaseDeDatos);

    if (!db.open()) {
        qDebug() << "Error al abrir la conexión a la base de datos: " << db.lastError().text();
        return;
    }

    // Crea la tabla si no existe
    QSqlQuery query;
    if (query.exec("CREATE TABLE IF NOT EXISTS "+acttema+" ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, "
                   "tipo INT, "
                   "texto TEXT, "
                   "estado BOOL, "
                   "porciento INT)")) {
    } else {
        qDebug() << "Error al crear la tabla 'tareas': " << query.lastError().text();
    }

    db.close();
}

void AdeT::obtenertablenombres() {
    ui->tem->clear();
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "La base de datos no está abierta.";
        return;
    }

    QStringList nombresTablas;
    QString consultaNombresTablas = "SELECT name FROM sqlite_master WHERE type='table'";

    QSqlQuery query;
    query.prepare(consultaNombresTablas);
    if (!query.exec()) {
        qDebug() << "Error al ejecutar la consulta: " << query.lastError().text();
        return;
    }

    while (query.next()) {
        nombresTablas.append(query.value(0).toString());
    }
    nombresTablas.removeAll("sqlite_sequence");
    ui->tem->addItems(nombresTablas);
}

void AdeT::obtenertareas() {

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "La base de datos no está abierta.";
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT id, tipo, texto, estado, porciento FROM "+tema);

    if (query.exec()) {
        while (query.next()) {
            tarea tar;
            tar.id = query.value(0).toInt();
            tar.tipo = query.value(1).toInt();
            tar.texto = query.value(2).toString();
            tar.estado = query.value(3).toBool();
            tar.porciento = query.value(4).toInt();
            //qDebug()<<query.value(0).toInt()<<query.value(1).toInt()<<query.value(2).toString()<<query.value(3).toBool()<<query.value(4).toInt();
            agregartarea(tar);
            id=query.value(0).toInt();
        }
    } else {
        qDebug() << "Error al obtener la lista de tareas: " << query.lastError().text();
    }
}

bool AdeT::insertar(tarea tar) {
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "La base de datos no está abierta.";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO "+tema+" (tipo, texto, estado, porciento) VALUES (?, ?, ?, ?)");
    query.addBindValue(tar.tipo);
    query.addBindValue(tar.texto);
    query.addBindValue(tar.estado);
    query.addBindValue(tar.porciento);

    if (!query.exec()) {
        qDebug() << "Error al insertar el usuario: " << query.lastError().text();
        return false;
    }
    return true;
}

void AdeT::eliminartareas(const int& id) {
    QSqlDatabase db = QSqlDatabase::database(); // Usa la instancia actual de la base de datos
    if (!db.isOpen()) {
        qDebug() << "La base de datos no está abierta.";
        return;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM "+tema+" WHERE id =?"); // Añade LIMIT 1 aquí
    query.addBindValue(id);

    if (!query.exec()) {
        qDebug() << "Error al eliminar la tarea: " << query.lastError().text();
        return;
    }
}

void AdeT::modificartarea(int id, bool estado, int porciento,int modificar) {
    QSqlDatabase db = QSqlDatabase::database(); // Usa la instancia actual de la base de datos
    if (!db.isOpen()) {
        qDebug() << "La base de datos no está abierta.";
        return;
    }

    QSqlQuery query;
    if (modificar==0){
        query.prepare("UPDATE "+tema+" SET estado = ? WHERE id = ?");
        query.addBindValue(estado);
        query.addBindValue(id);
    }else{
        query.prepare("UPDATE "+tema+" SET porciento = ? WHERE id = ?");
        query.addBindValue(porciento);
        query.addBindValue(id);
    }
         // Usuario actual para identificar el registro a actualizar

    if (!query.exec()) {
        qDebug() << "Error al actualizar la tarea:" << query.lastError();
    }
}

void AdeT::on_pushButton_clicked()
{
    ui->tema->setVisible(!ui->tema->isVisible());
    ui->crear->setVisible(!ui->crear->isVisible());
}


void AdeT::on_crear_clicked()
{
    ui->tema->setVisible(false);
    ui->crear->setVisible(false);
    crearBaseDatos(ui->tema->text());
    obtenertablenombres();
    ui->tem->setCurrentText(ui->tema->text());

}

void AdeT::on_tem_currentTextChanged(const QString &arg1)
{
    eliminarHijos();
    tema=arg1;
    obtenertareas();
}

void AdeT::on_del_clicked()
{
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.open()) {
        qDebug() << "Error al abrir la base de datos";
        return;
    }

    // Preparar la consulta SQL para eliminar la tabla
    QSqlQuery query;
    query.prepare("DROP TABLE IF EXISTS " + ui->tem->currentText());

    // Ejecutar la consulta
    if (!query.exec()) {
        qDebug() << "Error al eliminar la tabla: " << query.lastError();
    } else {
        qDebug() << "Tabla eliminada exitosamente.";
    }

    // Cerrar la conexión a la base de datos
    db.close();
    obtenertablenombres();
}

