/* PostMonster, universal HTTP automation tool
 * Copyright (C) 2015 by Paul Artsishevsky <polter.rnd@gmail.com>
 *
 * This file is part of PostMonster.
 *
 * PostMonster is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PostMonster is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PostMonster.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "requestform.h"
#include "ui_requestform.h"

#include <QMessageBox>

RequestForm::RequestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RequestForm), m_task(0)
{
    ui->setupUi(this);

    ui->bodySplitter->setStretchFactor(0, 1);
    ui->bodySplitter->setStretchFactor(1, 2);

    QHeaderView *headerView;

    ui->headersTable->setModel(&m_headersModel);
    headerView = ui->headersTable->horizontalHeader();
    headerView->setSectionResizeMode(HeadersModel::Name,  QHeaderView::ResizeToContents);
    headerView->setSectionResizeMode(HeadersModel::Value, QHeaderView::Stretch);

    ui->cookiesTable->setModel(&m_cookiesModel);
    headerView = ui->cookiesTable->horizontalHeader();
    headerView->hideSection(CookiesModel::Domain);
    headerView->setSectionResizeMode(CookiesModel::Name,  QHeaderView::ResizeToContents);
    headerView->setSectionResizeMode(CookiesModel::Value, QHeaderView::Stretch);

    connect(ui->nameEdit, SIGNAL(textChanged(QString)), this, SLOT(updateTask()));
    connect(ui->urlEdt, SIGNAL(textChanged(QString)), this, SLOT(updateTask()));
    connect(ui->bodyEdit, SIGNAL(textChanged()), this, SLOT(updateTask()));
    connect(ui->methodCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTask()));

    connect(ui->addCookieButton, SIGNAL(clicked()), this, SLOT(addCookie()));
    connect(ui->addHeaderButton, SIGNAL(clicked()), this, SLOT(addHeader()));
    connect(ui->removeCookiesButton, SIGNAL(clicked()), this, SLOT(removeCookies()));
    connect(ui->removeHeadersButton, SIGNAL(clicked()), this, SLOT(removeHeaders()));
}

void RequestForm::updateTask()
{
    if (!m_task) return;

    if (!m_task->setName(ui->nameEdit->text()))
        ui->nameEdit->undo();

    m_task->request()->url = ui->urlEdt->text();
    m_task->request()->body = ui->bodyEdit->toPlainText().toLatin1();
    m_task->request()->method = ui->methodCombo->currentText().toLatin1();

    emit m_task->dataChanged();
}

void RequestForm::addCookie()
{
    m_cookiesModel.add(QNetworkCookie());
    //ui->cookiesTable->setCurrentIndex(m_cookiesModel.rowCount() - 1);
    ui->cookiesTable->selectRow(m_cookiesModel.rowCount() - 1);
}

void RequestForm::addHeader()
{
    m_headersModel.add(QNetworkReply::RawHeaderPair());
}


void RequestForm::updateData(HttpTask *task)
{
    m_task = 0;

    QList<QNetworkReply::RawHeaderPair> *headers = &task->request()->headers;
    QList<QNetworkCookie> *cookies = &task->request()->cookies;

    m_headersModel.setHeaders(headers);
    for (int i = 0; i < m_headersModel.rowCount(); i++)
        ui->headersTable->showRow(i);
    ui->headersTable->resizeColumnToContents(HeadersModel::Name);

    m_cookiesModel.setCookies(cookies);
    ui->cookiesTable->resizeColumnToContents(CookiesModel::Name);

    /*ui->propertiesTabWidget->setTabText(
                1, ui->cookiesTab->property("nameFormat").toString()
                .arg(m_cookiesModel.rowCount()));*/

    ui->nameEdit->setText(task->name());
    ui->urlEdt->setText(task->request()->url);
    ui->bodyEdit->setText(task->request()->body);
    ui->methodCombo->setCurrentText(task->request()->method);

    ui->urlEdt->setCursorPosition(0);

    m_task = task;
}

void RequestForm::removeHeaders()
{
    QModelIndexList indexes = ui->headersTable->selectionModel()->selectedRows();
    QModelIndexList::const_iterator i = indexes.constEnd();
    while (i != indexes.constBegin()) {
        --i;
        m_headersModel.removeRow((*i).row());
    }
}

void RequestForm::removeCookies()
{
    QModelIndexList indexes = ui->cookiesTable->selectionModel()->selectedRows();
    QModelIndexList::const_iterator i = indexes.constEnd();
    while (i != indexes.constBegin()) {
        --i;
        m_cookiesModel.removeRow((*i).row());
    }
}

RequestForm::~RequestForm()
{
    delete ui;
}