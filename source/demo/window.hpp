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

#ifndef QTMOSQUITTO_DEMO_WINDOW_HPP
#define QTMOSQUITTO_DEMO_WINDOW_HPP

#include <QtWidgets>

#include "../qtmosquitto.hpp"

class Window : public QWidget
{
  Q_OBJECT
  public:
    Window(QWidget* parent = 0);
    virtual ~Window();
  
  private slots:
    void doConnect();
    void doSubscribe();
    void doUnsubscribe();
    void doPublish();
    
    void message(const QString& topic, const QByteArray& payload);
  
  
  
  private:
    QtMosquittoClient* mClient;
    QLineEdit* mServerEdit;
    QLineEdit* mUsernameEdit;
    QLineEdit* mPasswordEdit;
    QLineEdit* mSubscribeEdit;
    QLineEdit* mPublishTopicEdit;
    QLineEdit* mPublishPayloadEdit;
    QTableWidget* mMessageTable;
};

#endif
