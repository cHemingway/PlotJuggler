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

  virtual bool xmlSaveState(QDomDocument& doc,
                            QDomElement& parent_element) const override;

  virtual bool xmlLoadState(const QDomElement& parent_element) override;


private:
  bool _running;

  PJ::MessageParserPtr _parser;

  QSerialPort _serial;

  void pushSingleCycle();
};
