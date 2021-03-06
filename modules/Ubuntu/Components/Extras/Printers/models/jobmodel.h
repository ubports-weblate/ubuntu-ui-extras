/*
 * Copyright (C) 2017 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef USC_JOB_MODEL_H
#define USC_JOB_MODEL_H

#include "printers_global.h"
#include "backend/backend.h"
#include "printer/printerjob.h"
#include "printer/signalratelimiter.h"

#include <QAbstractListModel>
#include <QByteArray>
#include <QModelIndex>
#include <QObject>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QVariant>

class PrinterBackend;
class PrinterJob;
class PRINTERS_DECL_EXPORT JobModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit JobModel(QObject *parent = Q_NULLPTR);
    explicit JobModel(PrinterBackend *backend,
                      QObject *parent = Q_NULLPTR);
    ~JobModel();

    enum Roles
    {
        // Qt::DisplayRole holds job title
        IdRole = Qt::UserRole,
        CollateRole,
        ColorModelRole,
        CompletedTimeRole,
        CopiesRole,
        CreationTimeRole,
        DuplexRole,
        HeldRole,
        ImpressionsCompletedRole,
        LandscapeRole,
        MessagesRole,
        PrinterNameRole,
        PrintRangeRole,
        PrintRangeModeRole,
        ProcessingTimeRole,
        QualityRole,
        ReverseRole,
        SizeRole,
        StateRole,
        TitleRole,
        UserRole,
        LastStateMessageRole,
        LastRole = LastStateMessageRole,
    };

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    int count() const;

    Q_INVOKABLE QVariantMap get(const int row) const;
    QSharedPointer<PrinterJob> getJob(const QString &printerName, const int &id);

    void updateJobPrinter(QSharedPointer<PrinterJob> job, QSharedPointer<Printer> printer);
private:
    void addJob(QSharedPointer<PrinterJob> job);
    void removeJob(QSharedPointer<PrinterJob> job);
    void updateJob(QSharedPointer<PrinterJob> Job);

    PrinterBackend *m_backend;

    QList<QSharedPointer<PrinterJob>> m_jobs;
    SignalRateLimiter m_signalHandler;
private Q_SLOTS:
    void jobCreated(const QString &text, const QString &printer_uri,
                    const QString &printer_name, uint printer_state,
                    const QString &printer_state_reasons,
                    bool printer_is_accepting_jobs, uint job_id,
                    uint job_state, const QString &job_state_reasons,
                    const QString &job_name,
                    uint job_impressions_completed);
    void jobState(const QString &text, const QString &printer_uri,
                  const QString &printer_name, uint printer_state,
                  const QString &printer_state_reasons,
                  bool printer_is_accepting_jobs, uint job_id,
                  uint job_state, const QString &job_state_reasons,
                  const QString &job_name,
                  uint job_impressions_completed);
    void jobCompleted(const QString &text, const QString &printer_uri,
                      const QString &printer_name, uint printer_state,
                      const QString &printer_state_reasons,
                      bool printer_is_accepting_jobs, uint job_id,
                      uint job_state, const QString &job_state_reasons,
                      const QString &job_name,
                      uint job_impressions_completed);
    void jobSignalPrinterModified(const QString &printerName);
    void updateJob(QString printerName, int jobId, QMap<QString, QVariant> attributes);

Q_SIGNALS:
    void countChanged();
    void forceJobRefresh(const QString &printerName, const int jobId);
};

class PRINTERS_DECL_EXPORT JobFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit JobFilter(QObject *parent = Q_NULLPTR);
    ~JobFilter();

    void filterOnPrinterName(const QString &name);
    int count() const;
public Q_SLOTS:
    QVariantMap get(const int row) const;

    void filterOnActive();
    void filterOnQueued();
    void filterOnPaused();
protected:
    virtual bool filterAcceptsRow(
        int sourceRow, const QModelIndex &sourceParent) const override;
    virtual bool lessThan(const QModelIndex &source_left,
                          const QModelIndex &source_right) const;

Q_SIGNALS:
    void countChanged();

private Q_SLOTS:
    void onSourceModelChanged();
    void onSourceModelCountChanged();

private:
    QString m_printerName = QString::null;
    bool m_printerNameFilterEnabled = false;

    bool m_activeFilterEnabled = false;
    QSet<PrinterEnum::JobState> m_activeStates;

    bool m_queuedFilterEnabled = false;
    QSet<PrinterEnum::JobState> m_queuedStates;

    bool m_pausedFilterEnabled = false;
    QSet<PrinterEnum::JobState> m_pausedStates;
};

#endif // USC_JOB_MODEL_H
