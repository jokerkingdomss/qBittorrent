/*
 * Bittorrent Client using Qt4 and libtorrent.
 * Copyright (C) 2006  Christophe Dumez
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Contact : chris@qbittorrent.org
 */
#ifndef __BITTORRENT_H__
#define __BITTORRENT_H__

#include <QHash>
#include <QList>
#include <QPair>
#include <QStringList>

#include <libtorrent/session.hpp>
#include "qtorrenthandle.h"

using namespace libtorrent;

class downloadThread;
class deleteThread;
class QTimer;

class bittorrent : public QObject{
  Q_OBJECT

  private:
    session *s;
    QString scan_dir;
    QTimer *timerScan;
    QTimer *timerAlerts;
    bool DHTEnabled;
    downloadThread *downloader;
    QStringList supported_preview_extensions;
    QString defaultSavePath;
    QStringList torrentsToPauseAfterChecking;
    QStringList reloadingTorrents;
    QHash<QString, QList<long> > ETAstats;
    QHash<QString, long> ETAs;
    QHash<QString, QPair<size_type,size_type> > ratioData;
    QTimer *ETARefresher;
    QStringList fullAllocationModeList;
    QHash<QString, QList<QPair<QString, QString> > > trackersErrors;
    deleteThread *deleter;
    QStringList pausedTorrents;
    QStringList finishedTorrents;
    QStringList unfinishedTorrents;

  protected:
    QString getSavePath(QString hash);

  public:
    // Constructor / Destructor
    bittorrent();
    ~bittorrent();
    QTorrentHandle getTorrentHandle(QString hash) const;
    bool isPaused(QString hash) const;
    bool isFilePreviewPossible(QString fileHash) const;
    bool isDHTEnabled() const;
    float getPayloadDownloadRate() const;
    float getPayloadUploadRate() const;
    session_status getSessionStatus() const;
    int getListenPort() const;
    QStringList getTorrentsToPauseAfterChecking() const;
    long getETA(QString hash) const;
    bool inFullAllocationMode(QString hash) const;
    float getRealRatio(QString hash) const;
    session* getSession() const;
    QList<QPair<QString, QString> > getTrackersErrors(QString hash) const;
    bool receivedPausedAlert(QString hash) const;
    void printPausedTorrents();
    QStringList getFinishedTorrents() const;
    QStringList getUnfinishedTorrents() const;
    bool isFinished(QString hash) const;
    bool has_filtered_files(QString hash) const;

  public slots:
    void addTorrent(QString path, bool fromScanDir = false, QString from_url = QString());
    void downloadFromUrl(QString url);
    void downloadFromURLList(const QStringList& url_list);
    void deleteTorrent(QString hash, bool permanent = false);
    bool pauseTorrent(QString hash);
    bool resumeTorrent(QString hash);
    void enableDHT();
    void disableDHT();
    void saveDHTEntry();
    void saveFastResumeAndRatioData();
    void enableDirectoryScanning(QString scan_dir);
    void disableDirectoryScanning();
    void enablePeerExchange();
    void enableIPFilter(ip_filter filter);
    void disableIPFilter();
    void pauseAndReloadTorrent(QTorrentHandle h);
    void resumeUnfinishedTorrents();
    void updateETAs();
    void saveTorrentSpeedLimits(QString hash);
    void loadTorrentSpeedLimits(QString hash);
    void saveDownloadUploadForTorrent(QString hash);
    void loadDownloadUploadForTorrent(QString hash);
    void handleDownloadFailure(QString url, QString reason);
    void loadWebSeeds(QString fileHash);
    // Session configuration - Setters
    void setListeningPortsRange(std::pair<unsigned short, unsigned short> ports);
    void setMaxConnections(int maxConnec);
    void setDownloadRateLimit(long rate);
    void setUploadRateLimit(long rate);
    void setGlobalRatio(float ratio);
    void setDHTPort(int dht_port);
    void setProxySettings(proxy_settings proxySettings, bool trackers=true, bool peers=true, bool web_seeds=true, bool dht=true);
    void setSessionSettings(session_settings sessionSettings);
    void setDefaultSavePath(QString savepath);
    void applyEncryptionSettings(pe_settings se);
    void loadFilesPriorities(QTorrentHandle& h);
    void setDownloadLimit(QString hash, long val);
    void setUploadLimit(QString hash, long val);
    void setUnfinishedTorrent(QString hash);
    void setFinishedTorrent(QString hash);

  protected slots:
    void scanDirectory();
    void readAlerts();
    void processDownloadedFile(QString, QString);
    bool loadTrackerFile(QString hash);
    void saveTrackerFile(QString hash);
    void reloadTorrent(const QTorrentHandle &h); // This is protected now, call pauseAndReloadTorrent() instead

  signals:
    void invalidTorrent(QString path);
    void duplicateTorrent(QString path);
    void addedTorrent(QString path, QTorrentHandle& h, bool fastResume);
    void finishedTorrent(QTorrentHandle& h);
    void fullDiskError(QTorrentHandle& h);
    void trackerError(QString hash, QString time, QString msg);
    void portListeningFailure();
    void trackerAuthenticationRequired(QTorrentHandle& h);
    void scanDirFoundTorrents(const QStringList& pathList);
    void newDownloadedTorrent(QString path, QString url);
    void aboutToDownloadFromUrl(QString url);
    void updateFileSize(QString hash);
    void peerBlocked(QString);
    void downloadFromUrlFailure(QString url, QString reason);
    void fastResumeDataRejected(QString name);
    void urlSeedProblem(QString url, QString msg);
    void torrentFinishedChecking(QString hash);

};

#endif
