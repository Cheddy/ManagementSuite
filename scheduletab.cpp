#include "scheduletab.h"
#include "ui_scheduletab.h"
#include <QtCore>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QPainter>
#include <QtGui>
#include "iostream"
#include "mainwindow.h"


ScheduleTab::ScheduleTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScheduleTab)
{
    ui->setupUi(this);
    ui->trainingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->trainingTable->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));
    loadSavedTraining(MainWindow::workspace + "/schedules.csv");
}

ScheduleTab::~ScheduleTab()
{
    delete ui;
}

void ScheduleTab::on_addTrainingButton_clicked()
{
    ui->trainingTable->insertRow(0);
    
    QTableWidgetItem *name = new QTableWidgetItem(ui->nameSelectorBox->currentText(),QTableWidgetItem::Type);    
    name->setFlags(name->flags() & ~Qt::ItemIsEditable);
    
    QDate current = QDate::currentDate();
    QTableWidgetItem *lastTaken = new QTableWidgetItem(current.toString("dd/MM/yyyy"),QTableWidgetItem::Type);
    lastTaken->setData(Qt::EditRole, current);
    
    QTableWidgetItem *renewalPeriod = new QTableWidgetItem(0,QTableWidgetItem::Type);
    renewalPeriod->setData(Qt::EditRole, 0);        
    
    QDate currentDate = QDate::currentDate();    
    QTableWidgetItem *item = new QTableWidgetItem(currentDate.toString("dd/MM/yyyy"),QTableWidgetItem::Type);
    item->setData(Qt::EditRole, currentDate);    
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    ui->trainingTable->setSortingEnabled(false);
    ui->trainingTable->setItem(0,0,name);
    ui->trainingTable->setItem(0,3,lastTaken);
    ui->trainingTable->setItem(0,4,renewalPeriod);
    ui->trainingTable->setItem(0,5,item);
    recalculateRow(0);    
    ui->trainingTable->setSortingEnabled(true);    
    ui->trainingTable->repaint();
}

void ScheduleTab::onDataChanged(const QModelIndex& previous, const QModelIndex& current)
{
    if(ui->trainingTable->item(previous.row(), 3) != NULL && ui->trainingTable->item(previous.row(), 4) != NULL && (previous.column() == 4 || previous.column() == 3)){
        recalculateRow(previous.row());       
    }
}

void ScheduleTab::recalculateRow(int row)
{
    if(ui->trainingTable->item(row, 3) != NULL && ui->trainingTable->item(row, 4) != NULL){
        ui->trainingTable->setSortingEnabled(false);
        if(ui->trainingTable->item(row, 5) == NULL){
            QDate currentDate = QDate::currentDate();    
            QTableWidgetItem *item = new QTableWidgetItem(currentDate.toString("dd/MM/yyyy"),QTableWidgetItem::Type);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->trainingTable->setItem(row, 5, item);
        }
        ui->trainingTable->item(row,5)->setData(Qt::EditRole, ui->trainingTable->item(row, 3)->data(Qt::EditRole).toDate().addMonths(ui->trainingTable->item(row, 4)->data(Qt::EditRole).toInt()));
        if(ui->trainingTable->item(row,5)->data(Qt::EditRole).toDate() < QDate::currentDate()){
            ui->trainingTable->item(row,5)->setBackgroundColor(QColor(255,0,0));
        }else if (ui->trainingTable->item(row,5)->data(Qt::EditRole).toDate() < QDate::currentDate().addMonths(1)){
            ui->trainingTable->item(row,5)->setBackgroundColor(QColor(255,255,0));
        }else{
            ui->trainingTable->item(row,5)->setBackgroundColor(QColor(0,255,0));
        }
        ui->trainingTable->repaint(); 
        ui->trainingTable->setSortingEnabled(true);        
    }
}

void ScheduleTab::on_deleteTrainingButton_clicked()
{
    while(!ui->trainingTable->selectionModel()->selectedIndexes().isEmpty()){
        ui->trainingTable->removeRow(ui->trainingTable->selectionModel()->selectedIndexes().first().row());
    }
}

void ScheduleTab::on_saveTrainingButton_clicked()
{
    QFile f(MainWindow::workspace + "/schedules.csv");
    if (f.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream data( &f );
        for(int row =0; row < ui->trainingTable->rowCount(); row++){
            for(int col =0; col < ui->trainingTable->columnCount() - 1; col++){
                QTableWidgetItem *item = ui->trainingTable->item(row,col);
                if(item != NULL){
                    data << item->text();
                }
                if(col != ui->trainingTable->columnCount() - 2)
                    data << ',';  
            }
            if(row != ui->trainingTable->rowCount() - 1)
                data << '\n';
        }
        f.close();
    }else{
        std::cerr << "Error!";
    }    
    
}

void ScheduleTab::loadSavedTraining(QString path)
{
    QFile f(path);
    if (f.open(QIODevice::ReadWrite))
    {
        QString fileData = f.readAll();
        if(!fileData.isEmpty()){
            QStringList lines = fileData.split(QString("\n"));
            QSet<QString> names;
            for(int row =0; row < lines.size(); row++){
                if(lines.at(row) != NULL){
                    ui->trainingTable->insertRow(row);                    
                    QStringList lineData = lines.at(row).split(QString(","));
                    for(int col =0; col < lineData.size(); col++){
                        if(lineData.at(col) != NULL){
                            QTableWidgetItem *item; 
                            if(col == 3){
                                QDate date = QDate::fromString(lineData.at(col), "yyyy-MM-dd");
                                item = new QTableWidgetItem(date.toString("dd/MM/yyyy"),QTableWidgetItem::Type);
                                item->setData(Qt::DisplayRole, date);                                
                                item->setData(Qt::EditRole, date);    
                            }else if(col == 4){
                                item = new QTableWidgetItem(QTableWidgetItem::Type);   
                                item->setData(Qt::DisplayRole, lineData.at(col).toInt());                                
                                item->setData(Qt::EditRole, lineData.at(col).toInt());                                
                            }else{
                                item = new QTableWidgetItem(lineData.at(col),QTableWidgetItem::Type);                            
                            }
                            if(col == 0 || col == 5){
                                item->setFlags(item->flags() & ~Qt::ItemIsEditable);               
                                if(col == 0){
                                    names << lineData.at(col);
                                }
                            }
                            
                            ui->trainingTable->setItem(row,col,item);
                        }
                    }
                }
            }
            ui->nameSelectorBox->addItems(names.toList());
        }
        f.close();
    }else{
        std::cerr << "Error!";
    }    
    if(QApplication::arguments().length() > 1 && QApplication::arguments().at(1) == "/boot"){
        QDate date = QDate::currentDate().addMonths(1);
        for(int i = 0; i < ui->trainingTable->rowCount(); i++){
            if(ui->trainingTable->item(i, 5)->data(Qt::EditRole).toDate() < date){
                MainWindow::outdatedSchedules = true;
            }
        }
    }
}

void ScheduleTab::on_nameSelectorBox_currentIndexChanged(const QString &name)
{
    if(name == QString("All Categories")){
        ui->addTrainingButton->setEnabled(false);
    }else{
        ui->addTrainingButton->setEnabled(true);
    }
    
    for(int i = 0; i < ui->trainingTable->rowCount(); i++){
        QTableWidgetItem *item = ui->trainingTable->item(i, 0);
        if(name == QString("All Categories")){
            ui->trainingTable->setRowHidden(i, false);            
        }else{
            if(item != NULL && item->text() == name){
                ui->trainingTable->setRowHidden(i, false);
            }else{
                ui->trainingTable->setRowHidden(i, true);            
            }
        }
    }
}

void ScheduleTab::on_addCategory(QString name){
    if(ui->nameSelectorBox->findText(name) < 0){
        ui->nameSelectorBox->addItem(name);
        ui->nameSelectorBox->setCurrentText(name);
    }else{
        std::cerr << "Category already exists!" << std::endl;
        ui->nameSelectorBox->setCurrentText(name);        
    }
}

void ScheduleTab::on_printButton_clicked()
{
    QString strStream;
    QTextStream out(&strStream);
    
    const int rowCount = ui->trainingTable->model()->rowCount();
    const int columnCount = ui->trainingTable->model()->columnCount();
    
    out <<  "<html>\n"
            "<head>\n"
            "<meta Content=\"Text/html; charset=Windows-1251\">\n"
         <<  QString("<title>%1</title>\n").arg("Schedule Table")
          <<  "</head>\n"
              "<body bgcolor=#ffffff link=#5000A0>\n"
              "<style>\n"
              "body {\n"
              "margin:0;\n"
              "padding:0;\n"
              "}\n"
              "</style>\n"
              "<table border=1 cellspacing=0 cellpadding=2>\n";
    
    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int column = 0; column < columnCount; column++)
        if (!ui->trainingTable->isColumnHidden(column))
            out << QString("<th>%1</th>").arg(ui->trainingTable->model()->headerData(column, Qt::Horizontal).toString());
    out << "</tr></thead>\n";
    
    // data table
    for (int row = 0; row < rowCount; row++) {
        if(!ui->trainingTable->isRowHidden(row)){
            out << "<tr>";
            for (int column = 0; column < columnCount; column++) {
                if (!ui->trainingTable->isColumnHidden(column)) {
                    QString color("#FFFFFF");
                    QString data("&nbsp;");
                    QTableWidgetItem *item = ui->trainingTable->item(row,column);
                    if(item != NULL){
                        color = QString(item->background().color().name());
                        if(color == "#000000"){
                            color = QString("#FFFFFF");
                        }
                        data = item->data(Qt::DisplayRole).toString();
                    }
                    out << QString("<td bgcolor=\'%2\'>%1</td>").arg(data).arg(color);
                }                    
            }
            out << "</tr>\n";
        }
    }               
    out <<  "</table>\n"
            "</body>\n"
            "</html>\n";
    
    QTextDocument *document = new QTextDocument();
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setPaperSize(QPrinter::A4);    
    QPrintDialog *dialog = new QPrintDialog(&printer, NULL);
    if (dialog->exec() == QDialog::Accepted) {
        document->setHtml(strStream);            
        document->print(&printer);
    }
    
    delete document;
}

void ScheduleTab::on_workspaceChanged()
{
    ui->trainingTable->clearContents();
    ui->trainingTable->setRowCount(0);    
    loadSavedTraining(MainWindow::workspace + "/schedules.csv");
}  

void ScheduleTab::on_requestLoadTrainingFile(QString name)
{
    importLegacyLog(name);
}

void ScheduleTab::importLegacyLog(QString path)
{
    QFile f(path);
    QFileInfo fileInfo(f.fileName());
    QString name = fileInfo.baseName().replace("_"," ");
    if (f.open(QIODevice::ReadWrite))
    {
        QString fileData = f.readAll();
        if(!fileData.isEmpty()){
            QStringList lines = fileData.remove("\r").split(QString("\n"));
            QSet<QString> names;
            for(int row =0; row < lines.size(); row++){
                if(lines.at(row) != NULL){
                    ui->trainingTable->insertRow(row);                    
                    QStringList lineData = lines.at(row).split(QString(","));
                    for(int col =0; col < 6; col++){
                        if(col == 0 || col == 3 || col == 4 || lineData.at(col - 1) != NULL){
                            QTableWidgetItem *item; 
                            if(col == 3){
                                QDate date = QDate::fromString(lineData.at(5), "dd-MM-yyyy");
                                if(date.isNull() || !date.isValid())
                                    date = QDate::fromString("01-01-2000", "dd-MM-yyyy");
                                item = new QTableWidgetItem(QTableWidgetItem::Type);
                                item->setData(Qt::DisplayRole, date);                                
                                item->setData(Qt::EditRole, date);    
                            }else if(col == 4){
                                QString tmp = lineData.at(6);
                                tmp.remove(QRegExp("[^0-9]"));
                                if(tmp.isEmpty())
                                    tmp = QString("0");
                                item = new QTableWidgetItem(QTableWidgetItem::Type);   
                                item->setData(Qt::DisplayRole, tmp.toInt());                                
                                item->setData(Qt::EditRole, tmp.toInt());                                
                            }else if(col == 0){
                                item = new QTableWidgetItem(name,QTableWidgetItem::Type);                            
                            }else if(col == 2){
                                item = new QTableWidgetItem(lineData.at(3),QTableWidgetItem::Type);
                            }else{
                                item = new QTableWidgetItem(lineData.at(1),QTableWidgetItem::Type);
                            }
                            if(col == 0 || col == 5){
                                item->setFlags(item->flags() & ~Qt::ItemIsEditable);               
                                if(col == 0){
                                    names << name;
                                }
                            }
                            ui->trainingTable->setItem(row,col,item);
                        }
                    }
                }
            }
            foreach(QString s, names){
                if(ui->nameSelectorBox->findText(s) < 0){
                    ui->nameSelectorBox->addItem(s);
                }
            }
        }
        f.close();
    }else{
        std::cerr << "Error!";
    }    
}
