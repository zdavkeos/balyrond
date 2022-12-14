#include "datastream_balyrond_dialog.h"
#include <QIntValidator>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QSerialPortInfo>

const QString DataStreamBalyrondDialog::prefix = "Balyrond::DataStreamBalyrond::";

DataStreamBalyrondDialog::DataStreamBalyrondDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataStreamBalyrondDialog)
{
    ui->setupUi(this);

    //----- restore the state of this Dialog from Settings ------

    QSettings settings;
    QString theme = settings.value("StyleSheet::theme", "light").toString();

    QString port = settings.value(prefix + "port", "COM1").toString();

    ui->lineEditPort->setText(port);

    QString txt("Ports Found:\n");
    auto ports = QSerialPortInfo::availablePorts();
    for (auto port : ports)
    {
        txt += "-  " + port.systemLocation() + "\n";
    }
    ui->portsLabel->setText(txt);

    restoreGeometry(settings.value(prefix + "geometry").toByteArray());
}

DataStreamBalyrondDialog::~DataStreamBalyrondDialog()
{
    QSettings settings;
    settings.setValue(prefix + "geometry", saveGeometry());

    delete ui;
}

void DataStreamBalyrondDialog::accept()
{
    //----- Save the state of this Dialog in Settings ------

    QSettings settings;

    settings.setValue(prefix + "port", ui->lineEditPort->text());

    QDialog::accept();
}

DataStreamBalyrondDialog::Params DataStreamBalyrondDialog::getParams() const
{
    Params params;

    params.port = ui->lineEditPort->text();

    return params;
}
