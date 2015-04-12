/*
Copyright (c) 2015 Silas Parker <skyhisi@gmail.com>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License v1.0
and Eclipse Distribution License v1.0 which accompany this distribution.
 
The Eclipse Public License is available at
   http://www.eclipse.org/legal/epl-v10.html
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.
 
Contributors:
   Silas Parker
*/

#ifndef QTMOSQUITTO_HPP
#define QTMOSQUITTO_HPP

#if defined (_WIN32) 
  #if defined(qtmosquitto_EXPORTS)
    #define  QTMOSQUITTO_EXPORT __declspec(dllexport)
  #else
    #define  QTMOSQUITTO_EXPORT __declspec(dllimport)
  #endif /* QTMOSQUITTO_EXPORT */
#else /* defined (_WIN32) */
 #define QTMOSQUITTO_EXPORT
#endif

#include <QtCore>
struct mosquitto;
struct mosquitto_message;

class QTMOSQUITTO_EXPORT QtMosquittoApp
{
  public:
    QtMosquittoApp();
    ~QtMosquittoApp();
};

class QTMOSQUITTO_EXPORT QtMosquittoClient : public QObject
{
  Q_OBJECT
  public:
    enum ClientError
    {
      UnknownError = -1,
      ConnectionRefusedProtocolVersion,
      ConnectionRefusedIdentifierRejected,
      ConnectionRefusedBrokerUnavailable,
      UnexpectedDisconnect
    };
  
    QtMosquittoClient(const QString& id = QString(), bool clean_session = true, QObject* parent = 0);
    virtual ~QtMosquittoClient();
  
    void setUsernamePassword(const QString& username, const QString& password);
    
    bool doConnect(const QString& host, int port = 1883, int keepalive = 60);
    
    int publish(const QString& topic, const QString& payload, int qos = 0, bool retain = false);
    int publish(const QString& topic, const QByteArray& payload, int qos = 0, bool retain = false);
    
    bool subscribe(const QString& topic, int qos = 0);
    bool unsubscribe(const QString& topic);
  
  public slots:
    bool doReconnect();
    bool doDisconnect();
    void setAutoReconnect(bool reconnect);
  
  signals:
    void connected();
    void disconnected();
    void connectState(bool connected);
    void error(ClientError clientError);
    void message(const QString& topic, const QByteArray& payload);
  
  private slots:
    void process();
  
  private:
    void connect_cb(int rc);
    static void connect_cb_s(struct mosquitto*, void* obj, int rc);
    void disconnect_cb(int rc);
    static void disconnect_cb_s(struct mosquitto*, void* obj, int rc);
    static void log_cb_s(struct mosquitto*,void* obj, int level, const char* str);
    void message_cb(const QString& topic, const QByteArray& payload);
    static void message_cb_s(struct mosquitto*,void* obj,const struct mosquitto_message* msg);
    struct data;
    data* d;
    Q_DISABLE_COPY(QtMosquittoClient)
};

#endif
