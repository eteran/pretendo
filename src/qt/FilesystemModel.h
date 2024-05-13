/*
Copyright (C) 2014 - 2015 Evan Teran
                          evan.teran@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FILESYSTEM_MODEL_H_20191119_
#define FILESYSTEM_MODEL_H_20191119_

#include <QAbstractItemModel>
#include <QDirIterator>
#include <QString>
#include <QVector>

class IProcess;

class FilesystemModel final : public QAbstractItemModel {
	Q_OBJECT

public:
	struct Item {
		QString filename;
		QString path;
	};

public:
	explicit FilesystemModel(QObject *parent = nullptr);
	~FilesystemModel() override = default;

public:
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

public Q_SLOTS:
	void addFile(const Item &item);
	void clear();

private:
	QVector<Item> items_;
};

#endif
