#pragma once

#include <QDialog>
#include "ui_datastream_balyrond_dialog.h"

class DataStreamBalyrondDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataStreamBalyrondDialog(QWidget *parent = nullptr);

    ~DataStreamBalyrondDialog() override;

    struct Params
    {
        QString port;
    };

    Params getParams() const;

    static const QString prefix;

private slots:

    void accept() override;

private:
    Ui::DataStreamBalyrondDialog *ui;

};
