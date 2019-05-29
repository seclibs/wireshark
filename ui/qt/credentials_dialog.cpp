/*
 * credentials_dialog.c
 *
 * Copyright 2019 - Dario Lombardo <lomato@gmail.com>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <config.h>

#include "file.h"

#include "credentials_dialog.h"
#include <ui_credentials_dialog.h>
#include <ui/tap-credentials.h>
#include "wireshark_application.h"
#include "ui/qt/widgets/wireshark_file_dialog.h"
#include "ui/qt/models/credentials_model.h"
#include <ui/qt/models/url_link_delegate.h>

#include <QClipboard>
#include <QMessageBox>
#include <QPushButton>
#include <QTextCursor>

#include <QDebug>

class CredentialsUrlDelegate : public UrlLinkDelegate
{
public:

    CredentialsUrlDelegate(QObject * parent) : UrlLinkDelegate(parent) {}

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        bool ok = false;
        int val = index.data(Qt::UserRole).toInt(&ok);
        if (!ok || val <= 0)
            QStyledItemDelegate::paint(painter, option, index);
        else
            UrlLinkDelegate::paint(painter, option, index);
    }

};

CredentialsDialog::CredentialsDialog(QWidget &parent, CaptureFile &cf, PacketList *packet_list) :
    WiresharkDialog(parent, cf),
    ui(new Ui::CredentialsDialog)
{
    ui->setupUi(this);
    packet_list_ = packet_list;

    CredentialsModel* model = new CredentialsModel(this, cf);
    ui->auths->setModel(model);

    setWindowSubtitle(tr("Credentials"));

    ui->auths->setRootIsDecorated(false);
    ui->auths->setItemDelegateForColumn(CredentialsModel::COL_NUM, new CredentialsUrlDelegate(this));
    ui->auths->setItemDelegateForColumn(CredentialsModel::COL_USERNAME, new CredentialsUrlDelegate(this));

    ui->auths->resizeColumnToContents(CredentialsModel::COL_NUM);
    ui->auths->resizeColumnToContents(CredentialsModel::COL_PROTO);
    ui->auths->resizeColumnToContents(CredentialsModel::COL_USERNAME);

    ui->auths->setSortingEnabled(true);

    connect(ui->auths, SIGNAL(clicked(const QModelIndex&)), this, SLOT(actionGoToPacket(const QModelIndex&)));
}

CredentialsDialog::~CredentialsDialog()
{
    delete ui;
}

void CredentialsDialog::actionGoToPacket(const QModelIndex& idx)
{
    if (!idx.isValid())
        return;

    QVariant packet_data = idx.data(Qt::UserRole);
    QVariant hf_id = idx.data(CredentialsModel::ColumnHFID);
    if (!hf_id.canConvert(QVariant::Int))
        hf_id = qVariantFromValue(0);

    if (packet_data.canConvert(QVariant::Int))
        packet_list_->goToPacket(packet_data.toInt(), hf_id.toInt());
}
/*
 * Editor modelines
 *
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * ex: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
