#include "datastream_balyrond.h"
#include <chrono>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QSettings>

using namespace PJ;

DataStreamBalyrond::DataStreamBalyrond()
{

}

bool DataStreamBalyrond::start(QStringList*)
{
    if (_running)
    {
        return _running;
    }

    //--- Run the dialog to update the _params ---

    DataStreamBalyrondDialog* dialog = new DataStreamBalyrondDialog(nullptr);
    auto res = dialog->exec();

    if( res != QDialog::Accepted )
    {
        return false;
    }
    _params = dialog->getParams();


    _port= new QSerialPort(this);
    // connect(_port, &QSerialPort::readyRead, this, &DataStreamBalyrond::process);

    _port->setPortName(_params.port);
    _port->setBaudRate(115200);
    
    if (!_port->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(nullptr, tr("Could not open port"),
                             QString("Couldn't connect"),
                             QMessageBox::Ok);
        return false;
    }

    _input_data.clear();
    
    _input_data.emplace_back(&dataMap().getOrCreateNumeric("angle"));
    _input_data.emplace_back(&dataMap().getOrCreateNumeric("distance"));
    _input_data.emplace_back(&dataMap().getOrCreateNumeric("velocity"));
    
    _running = true;
    _thread = std::thread([this]() { this->receiveLoop(); });

    dialog->deleteLater();

    return true;
}

void DataStreamBalyrond::shutdown()
{
    if (_running)
    {
        _port->close();
        _running = false;
    }
}

DataStreamBalyrond::~DataStreamBalyrond()
{
    shutdown();
}

bool DataStreamBalyrond::xmlSaveState(QDomDocument &doc, QDomElement &parent_element) const
{
    auto options = doc.createElement("options");
    options.setAttribute("port", _params.port);

    parent_element.appendChild(options);
    return true;
}

bool DataStreamBalyrond::xmlLoadState(const QDomElement &parent_element)
{
    QDomElement options = parent_element.firstChildElement("options");

    QSettings settings;

    settings.setValue(DataStreamBalyrondDialog::prefix + "port", _params.port);

    return true;
}

void DataStreamBalyrond::receiveLoop()
{
    double i = 0;
    while (_running)
    {
        try
        {
            if (_port->waitForReadyRead())
            {
                if (_port->canReadLine())
                {
                    auto data = _port->readLine();

                    std::lock_guard<std::mutex> lock(mutex());

                    QString line = QString::fromUtf8(data);
                    auto parts = line.split(",");

                    if (parts.size() == 3)
                    {
                        _input_data[0]->pushBack({i, parts[0].toDouble()});
                        _input_data[1]->pushBack({i, parts[1].toDouble()});
                        _input_data[2]->pushBack({i, parts[2].toDouble()});
                        i += 1.0 / 500.0; // approx
                    }
                }
            }
        }
        catch (std::exception& err)
        {
            QMessageBox::warning(nullptr, tr("Balyrond Streaming Plugin"),
                                 tr("Problem parsing message. Stopping.\n%1")
                                 .arg(err.what()),
                                 QMessageBox::Ok);

            shutdown();
            // notify the GUI
            emit closed();
            return;
        }

        emit this->dataReceived();
    }
}


