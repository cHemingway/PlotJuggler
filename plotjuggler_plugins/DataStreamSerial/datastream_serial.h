#pragma once

#include <QtPlugin>
#include <QSerialPort>
#include <thread>
#include "PlotJuggler/datastreamer_base.h"

class DataStreamSerial : public PJ::DataStreamer
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "facontidavide.PlotJuggler3.DataStreamer")
  Q_INTERFACES(PJ::DataStreamer)

public:
  DataStreamSerial();

  virtual bool start(QStringList*) override;

  virtual void shutdown() override;

  virtual bool isRunning() const override;

  virtual ~DataStreamSerial() override;

  virtual const char* name() const override
  {
    return "Serial Streamer";
  }

  virtual bool isDebugPlugin() override
  {
    return false;
  }


private:
  bool _running;

  PJ::MessageParserPtr _parser;

  QSerialPort _serial;

private slots:
  void processSerial();

  void serialError(QSerialPort::SerialPortError error);
  
};
