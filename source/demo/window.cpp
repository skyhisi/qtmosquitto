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

#include "window.hpp"

Window::Window(QWidget* par) :
  QWidget(par),
  mClient(new QtMosquittoClient(QString(), true, this)),
  mServerEdit(new QLineEdit()),
  mUsernameEdit(new QLineEdit()),
  mPasswordEdit(new QLineEdit()),
  mSubscribeEdit(new QLineEdit()),
  mPublishTopicEdit(new QLineEdit()),
  mPublishPayloadEdit(new QLineEdit()),
  mMessageTable(new QTableWidget())
{
  setWindowTitle(tr("QtMosquitto Demo"));
  resize(800, 600);
  
  mClient->setAutoReconnect(true);
  
  
  QPushButton* connectButton = new QPushButton(tr("Connect"));
  QPushButton* disconnectButton = new QPushButton(tr("Disconnect"));
  disconnectButton->setEnabled(false);
  QHBoxLayout* connectButtonLayout = new QHBoxLayout();
  connectButtonLayout->addWidget(connectButton);
  connectButtonLayout->addWidget(disconnectButton);
  
  mPasswordEdit->setEchoMode(QLineEdit::Password);
  
  QFormLayout* serverGroupLayout = new QFormLayout();
  serverGroupLayout->addRow(tr("Server"), mServerEdit);
  serverGroupLayout->addRow(tr("Username"), mUsernameEdit);
  serverGroupLayout->addRow(tr("Password"), mPasswordEdit);
  serverGroupLayout->addRow(connectButtonLayout);
  
  QGroupBox* serverGroup = new QGroupBox(tr("Server"));
  serverGroup->setLayout(serverGroupLayout);
  
  
  QPushButton* publishButton = new QPushButton(tr("Publish"));
  QFormLayout* publishFormLayout = new QFormLayout();
  publishFormLayout->addRow(tr("Topic"), mPublishTopicEdit);
  publishFormLayout->addRow(tr("Payload"), mPublishPayloadEdit);
  publishFormLayout->addRow(publishButton);
  
  QGroupBox* publishGroup = new QGroupBox(tr("Publish Message"));
  publishGroup->setLayout(publishFormLayout);
  
  
  
  QPushButton* subscribeButton = new QPushButton(tr("Subscribe"));
  QPushButton* unsubscribeButton = new QPushButton(tr("Unsubscribe"));
  QHBoxLayout* subscriptionLayout = new QHBoxLayout();
  subscriptionLayout->addWidget(new QLabel(tr("Topic")), 0);
  subscriptionLayout->addWidget(mSubscribeEdit, 1);
  subscriptionLayout->addWidget(subscribeButton, 0);
  subscriptionLayout->addWidget(unsubscribeButton, 0);
  
  QGroupBox* subscriptionGroup = new QGroupBox(tr("Subscriptions"));
  subscriptionGroup->setLayout(subscriptionLayout);
  
  mMessageTable->setColumnCount(2);
  mMessageTable->setHorizontalHeaderLabels((QStringList() << tr("Topic") << tr("Payload")));
  
  QVBoxLayout* messageLayout = new QVBoxLayout();
  messageLayout->addWidget(mMessageTable);
  
  QGroupBox* messageGroup = new QGroupBox(tr("Messages"));
  messageGroup->setLayout(messageLayout);
  
  
  QVBoxLayout* mainLayout = new QVBoxLayout();
  mainLayout->addWidget(serverGroup, 0);
  mainLayout->addWidget(publishGroup, 0);
  mainLayout->addWidget(subscriptionGroup, 0);
  mainLayout->addWidget(messageGroup, 1);
  
  setLayout(mainLayout);
  
  connect(connectButton, SIGNAL(clicked()), this, SLOT(doConnect()));
  connect(disconnectButton, SIGNAL(clicked()), mClient, SLOT(doDisconnect()));
  connect(publishButton, SIGNAL(clicked()), this, SLOT(doPublish()));
  connect(subscribeButton, SIGNAL(clicked()), this, SLOT(doSubscribe()));
  connect(unsubscribeButton, SIGNAL(clicked()), this, SLOT(doUnsubscribe()));
  

  connect(mClient, SIGNAL(connectState(bool)), connectButton, SLOT(setDisabled(bool)));
  connect(mClient, SIGNAL(connectState(bool)), disconnectButton, SLOT(setEnabled(bool)));
  connect(mClient, SIGNAL(message(QString,QByteArray)), this, SLOT(message(QString,QByteArray)));
}

Window::~Window()
{
}

void Window::doConnect()
{
  mMessageTable->clearContents();
  
  const QString server = mServerEdit->text().trimmed();
  const QString uname = mUsernameEdit->text().trimmed();
  const QString pword = mPasswordEdit->text().trimmed();
  if (!(uname.isEmpty() || pword.isEmpty()))
  {
    mClient->setUsernamePassword(uname, pword);
  }
  mClient->doConnect(server);
}

void Window::doSubscribe()
{
  const QString topic = mSubscribeEdit->text().trimmed();
  if (!mClient->subscribe(topic))
  {
    QMessageBox::warning(this, tr("Subscribe"), tr("Subscribe failed"));
  }
}

void Window::doUnsubscribe()
{
  const QString topic = mSubscribeEdit->text().trimmed();
  if (!mClient->unsubscribe(topic))
  {
    QMessageBox::warning(this, tr("Unsubscribe"), tr("Unsubscribe failed"));
  }
}

void Window::doPublish()
{
  const QString topic = mPublishTopicEdit->text().trimmed();
  const QString payload = mPublishPayloadEdit->text().trimmed();
  
  if (mClient->publish(topic, payload) < 0)
  {
    QMessageBox::warning(this, tr("Publish"), tr("Publish failed"));
  }
}

void Window::message(const QString& topic, const QByteArray& payload)
{
  qDebug() << "Window::message" << topic << payload;
  mMessageTable->insertRow(0);
  mMessageTable->setItem(0, 0, new QTableWidgetItem(topic));
  mMessageTable->setItem(0, 1, new QTableWidgetItem(QString::fromUtf8(payload)));
  mMessageTable->setVerticalHeaderItem(0, new QTableWidgetItem(QDateTime::currentDateTime().toString(Qt::DefaultLocaleShortDate)));
}



