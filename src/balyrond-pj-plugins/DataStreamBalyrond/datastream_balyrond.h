#pragma once

#include <QtPlugin>
#include <QSerialPort>

#include <thread>
#include <chrono>

#include "PlotJuggler/datastreamer_base.h"
#include "PlotJuggler/dataloader_base.h"
#include "datastream_balyrond_dialog.h"

class DataStreamBalyrond : public PJ::DataStreamer
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "facontidavide.PlotJuggler3.DataStreamer")
    Q_INTERFACES(PJ::DataStreamer)

public:
    DataStreamBalyrond();

    virtual bool start(QStringList*) override;

    virtual void shutdown() override;

    virtual bool isRunning() const override
    {
        return _running;
    }

    virtual ~DataStreamBalyrond() override;

    virtual const char* name() const override
    {
        return "DataStream Balyrond";
    }

    virtual bool isDebugPlugin() override
    {
        return false;
    }

    virtual bool xmlSaveState(QDomDocument& doc, QDomElement& parent_element) const override;

    virtual bool xmlLoadState(const QDomElement& parent_element) override;

private:
    bool _running = false;
    std::thread _thread;

    QSerialPort* _port;
    DataStreamBalyrondDialog::Params _params;

    std::vector<PJ::PlotData*> _input_data;

    void receiveLoop();
};

