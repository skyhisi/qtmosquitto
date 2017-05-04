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

#include "qtmosquitto.hpp"

#include <mosquitto.h>

QtMosquittoApp::QtMosquittoApp()
{
  mosquitto_lib_init();
}

QtMosquittoApp::~QtMosquittoApp()
{
  mosquitto_lib_cleanup();
}

////////////////////////////////////////////////////////////////////////////////

struct QtMosquittoClient::data
{
  struct mosquitto*  mosq;
  bool autoreconnect;
  bool connected;
  QTimer processTimer;
  
  data():mosq(0),autoreconnect(false),connected(false),processTimer(){}
};


QtMosquittoClient::QtMosquittoClient(const QString& id, bool clean_session, QObject* par) :
  QObject(par),
  d(new data())
{
  QByteArray idBA(id.toUtf8());
  const char* idCC = (idBA.size() != 0) ? idBA.data() : 0;
  d->mosq = mosquitto_new(idCC, clean_session, this);
  Q_ASSERT(d->mosq);
  
  d->processTimer.setTimerType(Qt::CoarseTimer);
  d->processTimer.setSingleShot(false);
  d->processTimer.setInterval(100);
  connect(&d->processTimer, SIGNAL(timeout()), this, SLOT(process()));
  d->processTimer.start();
  
  mosquitto_connect_callback_set(d->mosq, &QtMosquittoClient::connect_cb_s);
  mosquitto_disconnect_callback_set(d->mosq, &QtMosquittoClient::disconnect_cb_s);
  mosquitto_log_callback_set(d->mosq, &QtMosquittoClient::log_cb_s);
  mosquitto_message_callback_set(d->mosq, &QtMosquittoClient::message_cb_s);
  
}

QtMosquittoClient::~QtMosquittoClient()
{
  d->processTimer.stop();
  if (d->connected)
  {
    mosquitto_disconnect(d->mosq);
    mosquitto_loop(d->mosq, 100, 1);
  }
  mosquitto_destroy(d->mosq);
  delete d;
  d = 0;
}

void QtMosquittoClient::setUsernamePassword(const QString& username, const QString& password)
{
  const QByteArray uname(username.toUtf8());
  const QByteArray pword(password.toUtf8());
  const int rc = mosquitto_username_pw_set(d->mosq, uname.data(), pword.data());
  Q_ASSERT(rc == MOSQ_ERR_SUCCESS);
}

bool QtMosquittoClient::doConnect(const QString& host, int port, int keepalive)
{
  if (d->connected)
  {
    qWarning() << "QtMosquittoClient::doConnect: Already connected";
    return false;
  }
  
  QByteArray hostBA(host.toUtf8());
  int rc = mosquitto_connect(d->mosq, hostBA.data(), port, keepalive);
  if (!(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING))
  {
    qWarning() << "QtMosquittoClient::doConnect: Failed to connect" << rc;
    return false;
  }
  d->connected = true;
  return true;
}

bool QtMosquittoClient::doConnect(const QString &cafile, const QString &certfile, const QString &keyfile, const QString &host, int port, int keepalive)
{
    if (d->connected)
    {
      qWarning() << "QtMosquittoClient::doConnect: Already connected";
      return false;
    }

    // Set TLS before connecting
    QByteArray cafileBA(cafile.toUtf8());
    QByteArray certfileBA(certfile.toUtf8());
    QByteArray keyfileBA(keyfile.toUtf8());
    int rt = mosquitto_tls_set(d->mosq, cafileBA.data(), NULL, certfileBA.data(), keyfileBA.data(), 0);
    if (!(rt == MOSQ_ERR_SUCCESS))
    {
      qWarning() << "QtMosquittoClient::doConnect: Failed to set TLS" << rt;
      return false;
    }

    // Connect
    QByteArray hostBA(host.toUtf8());
    int rc = mosquitto_connect(d->mosq, hostBA.data(), port, keepalive);
    if (!(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING))
    {
      qWarning() << "QtMosquittoClient::doConnect: Failed to connect" << rc;
      return false;
    }

    d->connected = true;
    return true;
}

bool QtMosquittoClient::doReconnect()
{
  if (d->connected)
  {
    qWarning() << "QtMosquittoClient::doReconnect: Already connected";
    return false;
  }
  
  int rc = mosquitto_reconnect(d->mosq);
  if (!(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_CONN_PENDING))
  {
    qWarning() << "QtMosquittoClient::doReconnect: Failed to reconnect" << rc;
    return false;
  }
  d->connected = true;
  return true;
}

bool QtMosquittoClient::doDisconnect()
{
  if (!d->connected)
  {
    qWarning() << "QtMosquittoClient::doDisconnect: Not connected";
    return false;
  }
  int rc = mosquitto_disconnect(d->mosq);
  if (rc != MOSQ_ERR_SUCCESS)
  {
    qWarning() << "QtMosquittoClient::doDisconnect: Disconnect failed";
    return false;
  }
  d->connected = false;
  emit disconnected();
  emit connectState(false);
  return true;
}

void QtMosquittoClient::setAutoReconnect(bool reconnect)
{
  d->autoreconnect = reconnect;
}


int QtMosquittoClient::publish(const QString& topic, const QString& payload, int qos, bool retain)
{
  return publish(topic, payload.toUtf8(), qos, retain);
}

int QtMosquittoClient::publish(const QString& topic, const QByteArray& payload, int qos, bool retain)
{
  const QByteArray topicBA(topic.toUtf8());
  int mid = -1;
  int rc = mosquitto_publish(d->mosq, &mid, topicBA.data(), payload.size(), payload.data(), qos, retain);
  if (rc == MOSQ_ERR_SUCCESS)
  {
    return mid;
  }
  else
  {
    qWarning() << "QtMosquittoClient::publish: Failed to publish:" << rc;
    return -1;
  }
}


bool QtMosquittoClient::subscribe(const QString& topic, int qos)
{
  const QByteArray topicBA(topic.toUtf8());
  int rc = mosquitto_subscribe(d->mosq, NULL, topicBA.data(), qos);
  if (rc == MOSQ_ERR_SUCCESS)
  {
    return true;
  }
  else
  {
    qWarning() << "QtMosquittoClient::subscribe: Failed to subscribe:" << topic << rc;
    return false;
  }
}

bool QtMosquittoClient::unsubscribe(const QString& topic)
{
  const QByteArray topicBA(topic.toUtf8());
  int rc = mosquitto_unsubscribe(d->mosq, NULL, topicBA.data());
  if (rc == MOSQ_ERR_SUCCESS)
  {
    return true;
  }
  else
  {
    qWarning() << "QtMosquittoClient::unsubscribe: Failed to unsubscribe:" << topic << rc;
    return false;
  }
}


void QtMosquittoClient::process()
{
  mosquitto_loop(d->mosq, 0, 1);
}


void QtMosquittoClient::connect_cb(int rc)
{
  if (rc == 0)
  {
    d->connected = true;
    emit connected();
    emit connectState(true);
  }
  else
  {
    d->connected = false;
    emit connectState(false);
    switch (rc)
    {
      case 1: emit error(ConnectionRefusedProtocolVersion); break;
      case 2: emit error(ConnectionRefusedIdentifierRejected); break;
      case 3: emit error(ConnectionRefusedBrokerUnavailable); break;
      default: emit error(UnknownError);
    }
  }
}

void QtMosquittoClient::connect_cb_s(struct mosquitto*, void* obj, int rc)
{
  QtMosquittoClient* self = static_cast<QtMosquittoClient*>(obj);
  Q_ASSERT(self != 0);
  self->connect_cb(rc);
}

void QtMosquittoClient::disconnect_cb(int rc)
{
  d->connected = false;
  emit disconnected();
  emit connectState(false);
  if (rc != 0)
  {
    qWarning() << "QtMosquittoClient::disconnect_cb rc: " << rc;
    emit error(UnexpectedDisconnect);
    if (d->autoreconnect)
    {
      doReconnect();
    }
  }
}

void QtMosquittoClient::disconnect_cb_s(struct mosquitto*, void* obj, int rc)
{
  QtMosquittoClient* self = static_cast<QtMosquittoClient*>(obj);
  Q_ASSERT(self != 0);
  self->disconnect_cb(rc);
}


void QtMosquittoClient::log_cb_s(struct mosquitto*,void*, int level, const char* str)
{
  switch (level)
  {
    case MOSQ_LOG_DEBUG:   qDebug() << "MQTT(D):" << str; break;
    case MOSQ_LOG_INFO:    qDebug() << "MQTT(I):" << str; break;
    case MOSQ_LOG_NOTICE:  qDebug() << "MQTT(N):" << str; break;
    case MOSQ_LOG_WARNING: qWarning() << "MQTT(W):" << str; break;
    case MOSQ_LOG_ERR:     qWarning() << "MQTT(E):" << str; break;
    default: qDebug() << "MQTT(U):" << str; break;
  }
}

void QtMosquittoClient::message_cb(const QString& topic, const QByteArray& payload)
{
  emit message(topic, payload);
}

void QtMosquittoClient::message_cb_s(struct mosquitto*,void* obj,const struct mosquitto_message* msg)
{
  QtMosquittoClient* self = static_cast<QtMosquittoClient*>(obj);
  Q_ASSERT(self != 0);
  Q_ASSERT(msg != 0);
  QString topic(QString::fromUtf8(msg->topic));
  QByteArray data(static_cast<char*>(msg->payload), msg->payloadlen);
  self->message_cb(topic, data);
}
