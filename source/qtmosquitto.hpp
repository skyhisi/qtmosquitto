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
   Marco Pellin
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

/** Manage the initialisation and clean-up of the Mosquitto library.
 * An object of this class should be created on the stack in main() before any
 * other Mosquitto functions are used.
 */
class QTMOSQUITTO_EXPORT QtMosquittoApp
{
  public:
    /// Initialise the Mosquitto library
    QtMosquittoApp();

    /// Clean-up the Mosquitto library
    ~QtMosquittoApp();
};


/** MQTT client connection to server.
 * Wrap the client functions in Mosquitto to provide a client connection to the
 * server.
 */
class QTMOSQUITTO_EXPORT QtMosquittoClient : public QObject
{
  Q_OBJECT
  public:
    /// Error types emitted with the error() signal.
    enum ClientError
    {
      UnknownError = -1,
      ConnectionRefusedProtocolVersion,
      ConnectionRefusedIdentifierRejected,
      ConnectionRefusedBrokerUnavailable,
      UnexpectedDisconnect
    };

    /** Create the client.
     * \param id             Client ID - up to 23 characters to use as the
     *                           client ID, if empty a random ID will be
     *                           generated.
     * \param clean_session  If true a clean session will be created, must
     *                           be true if client ID is not set.
     * \param parent         QObject parent.
     */
    QtMosquittoClient(const QString& id = QString(), bool clean_session = true, QObject* parent = 0);

    /// Disconnect the client and release resources.
    virtual ~QtMosquittoClient();

    /** Set the username and password for authentication.
     * If the server requires authentication, call this before doConnect().
     */
    void setUsernamePassword(const QString& username, const QString& password);

    /** Configure the client for certificate based SSL/TLS support.
     * Must be called before doConnect.
     * \param cafile     Path to a file containing the PEM encoded trusted CA certificate files.
     *                   Either cafile or capath must not be NULL.
     * \param certfile   Path to a file containing the PEM encoded certificate file for this client.
     *                   If NULL, keyfile must also be NULL and no client certificate will be used.
     * \param keyfile    Path to a file containing the PEM encoded private key for this client.
     *                   If NULL, certfile must also be NULL and no client certificate will be used.
     * \returns True if configuration was accepted, false otherwise.
     */
    bool configureTLS(const QString &cafile, const QString &certfile, const QString &keyfile);

    /** Set advanced SSL/TLS options.
     * Must be called before doConnect.
     * \param cert_reqs     An integer defining the verification requirements the client will impose on the server.
     *                      This can be one of:
     *                      SSL_VERIFY_NONE (0): the server will not be verified in any way. Using SSL_VERIFY_NONE provides no security.
     *                      SSL_VERIFY_PEER (1): the server certificate will be verified and the connection aborted if the verification fails.
     *                      The default and recommended value is SSL_VERIFY_PEER.
     * \param tls_version   The version of the SSL/TLS protocol to use as a string.  If NULL, the default value is used.
     *                      The default value and the available values depend on the version of openssl that the library was compiled against.
     * \param ciphers       A string describing the ciphers available for use.  See the “openssl ciphers” tool for more information.
     *                      If NULL, the default ciphers will be used.
     * \returns True if configuration was accepted, false otherwise.
     */
    bool advancedOptionsTLS(int cert_reqs, const QString &tls_version, const QString &ciphers);

    /** Start connecting to the server.
     * Start connecting to the server, the connection will not have completed
     * before the call returns.
     * \param host       Host name or IP address of server.
     * \param port       Port on server running MQTT service.
     * \param keepalive  Interval between ping messages.
     * \returns True if connection is starting, false otherwise.
     * \sa connected, disconnected, connectState, error
     */
    bool doConnect(const QString& host, int port = 1883, int keepalive = 60);

    /** Publish a message to the server.
     * \param topic     Topic of message, e.g a/b/c
     * \param payload   Payload of message as string.
     * \param qos       Message QoS level.
     * \param retain    Flag to indicate server should hold message.
     * \returns Message ID on success or -1 on failure.
     */
    int publish(const QString& topic, const QString& payload, int qos = 0, bool retain = false);

    /** Publish a message to the server.
     * \param topic     Topic of message, e.g a/b/c
     * \param payload   Payload of message as binary data.
     * \param qos       Message QoS level.
     * \param retain    Flag to indicate server should hold message.
     * \returns Message ID on success or -1 on failure.
     */
    int publish(const QString& topic, const QByteArray& payload, int qos = 0, bool retain = false);

    /** Subscribe to messages with the given topic.
     * \param topic  Message topic to receive, wildcards are + for a single
     *                   level and # for multilevel.
     * \param qos    Message QoS.
     * \returns True if subscribe sent, false otherwise.
     * \sa QtMosquittoClient::message
     */
    bool subscribe(const QString& topic, int qos = 0);

    /** Unsubscribe from messages with the given topic.
     * \param[in] topic  Message topic to no longer receive.
     * \returns True if unsubscribe sent, false otherwise.
     */
    bool unsubscribe(const QString& topic);

  public slots:
    /** Reconnect to a server when the client has been disconnected.
     * \returns True if connection is restarting, false otherwise.
     */
    bool doReconnect();

    /** Disconnect from the server.
     * Send a disconnect message to the server, this may not be acknowledged and
     * the disconnected signal may not be emitted.
     * \returns True if disconnect message is sent, false otherwise.
     */
    bool doDisconnect();

    /** Enable automatic reconnect mode.
     * Enable an automatic reconnect when the connection has failed.
     */
    void setAutoReconnect(bool reconnect);

  signals:
    /** Emitted when the client has connected.
     * The signal handler for this is a good place to perform subscriptions.
     */
    void connected();

    /** Emitted when the client has disconnected. */
    void disconnected();

    /** True when client connected, false when disconnected. */
    void connectState(bool connected);

    /** Emitted when an error occurs. */
    void error(ClientError clientError);

    /** Emitted when a message is received for a subscription.
     * \param topic    Message topic, this is the full topic, even if the
     *                 subscription contained a wildcard.
     * \param payload  Message payload.
     */
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
