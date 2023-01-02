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


    _port = new QSerialPort(this);
    // connect(_port, &QSerialPort::readyRead, this, &DataStreamBalyrond::process);

    _port->setPortName(_params.port);
    _port->setBaudRate(115200);
    _port->setReadBufferSize(1024); // Breaks on mac os without this...?
    
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
        _running = false;

        if (_thread.joinable())
        {
            _thread.join();
        }

        if (_port)
        {
            _port->close();
        }

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
    while (_running)
    {
        try
        {
            if (_port->waitForReadyRead(1000))
            {
                QByteArray data = _port->readLine();

                using namespace std::chrono;
                auto ts = high_resolution_clock::now().time_since_epoch();
                double timestamp = 1e-6 * double(duration_cast<microseconds>(ts).count());

                // qDebug() << data;

                QString line = QString::fromUtf8(data);
                auto parts = line.split(",");

                if (parts.size() == 3)
                {
                    std::lock_guard<std::mutex> lock(mutex());
                    _input_data[0]->pushBack({timestamp, parts[0].toDouble()});
                    _input_data[1]->pushBack({timestamp, parts[1].toDouble()});
                    _input_data[2]->pushBack({timestamp, parts[2].toDouble()});
                }

                emit this->dataReceived();
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
    }
}


