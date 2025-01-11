#include "datastream_serial.h"
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QIntValidator>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <thread>
#include <mutex>
#include <chrono>
#include <thread>
#include <math.h>
#include "ui_datastream_serial.h"

using namespace PJ;

class SerialStreamDialog : public QDialog
{
public:
  SerialStreamDialog() : QDialog(nullptr), ui(new Ui::SerialDialog)
  {
    ui->setupUi(this);
    ui->comboBoxBaud->setValidator(new QIntValidator());
    setWindowTitle("Serial Port");

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  }
  ~SerialStreamDialog()
  {
    while (ui->layoutOptions->count() > 0)
    {
      auto item = ui->layoutOptions->takeAt(0);
      item->widget()->setParent(nullptr);
    }
    delete ui;
  }
  Ui::SerialDialog* ui;
};

DataStreamSerial::DataStreamSerial() : _running(false)
{
}

bool DataStreamSerial::start(QStringList*)
{
  if (_running)
  {
    return _running;
  }

  if (parserFactories() == nullptr || parserFactories()->empty())
  {
    QMessageBox::warning(nullptr, tr("Serial Client"), tr("No available MessageParsers"),
                         QMessageBox::Ok);
    _running = false;
    return false;
  }

  SerialStreamDialog dialog;

  // Add serial ports and baud rates to dialog
  for (const auto& it : QSerialPortInfo::availablePorts())
  {
    dialog.ui->comboBoxPort->addItem(it.portName());
  }
  for (const auto &it : QSerialPortInfo::standardBaudRates())
  {
    dialog.ui->comboBoxBaud->addItem(QString::number(it));
  }

  // Add parsers to dialog
  for (const auto& it : *parserFactories())
  {
    dialog.ui->comboBoxProtocol->addItem(it.first);

    if (auto widget = it.second->optionsWidget())
    {
      widget->setVisible(false);
      dialog.ui->layoutOptions->addWidget(widget);
    }
  }

  // load previous values
  QSettings settings;
  QString port_str = settings.value("Serial::port").toString();
  unsigned int baud = settings.value("Serial::baud").toUInt();
  QString protocol = settings.value("Serial::protocol").toString();
  if (parserFactories()->find(protocol) == parserFactories()->end())
  {
    protocol = "json";
  }

  // Create and show dialog
  int res = dialog.exec();
  if (res == QDialog::Rejected)
  {
    _running = false;
    return false;
  }

  // Load fields from dialog
  port_str = dialog.ui->comboBoxPort->currentText();
  protocol = dialog.ui->comboBoxProtocol->currentText();
  baud = dialog.ui->comboBoxBaud->currentText().toUInt();

  ParserFactoryPlugin::Ptr parser_creator;
  _parser = parser_creator->createParser({}, {}, {}, dataMap());

  // Save back to settings
  settings.setValue("Serial::port", port_str);
  settings.setValue("Serial::baud", baud);
  settings.setValue("Serial::protocol", protocol);


  // Open the serial port
  _serial.setPortName(port_str);
  _serial.setBaudRate(baud);
  _serial.setDataBits(QSerialPort::Data8);
  _serial.setParity(QSerialPort::NoParity);
  _serial.setStopBits(QSerialPort::OneStop);
  _serial.setFlowControl(QSerialPort::NoFlowControl);

  if (!_serial.open(QIODevice::ReadOnly)) {
    QMessageBox::warning(nullptr, "Serial Client", 
                        QString("Failed to open serial port, error: %1").arg(_serial.errorString()),
                         QMessageBox::Ok);
    _running = false;
    return false;
  }

  // TODO: Start a thread that reads from the serial port

  _running = true;
  return _running;
}

void DataStreamSerial::shutdown()
{
  _serial.close();
  _running = false;
}

bool DataStreamSerial::isRunning() const
{
  return _running;
}

DataStreamSerial::~DataStreamSerial()
{
  shutdown();
}

bool DataStreamSerial::xmlSaveState(QDomDocument& doc, QDomElement& parent_element) const
{
  return true; // TODO
}

bool DataStreamSerial::xmlLoadState(const QDomElement& parent_element)
{
  return true; // TODO
}
