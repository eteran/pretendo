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

#include "FilesystemModel.h"

#include <QtAlgorithms>

FilesystemModel::FilesystemModel(QObject *parent)
	: QAbstractItemModel(parent) {
}

QModelIndex FilesystemModel::index(int row, int column, const QModelIndex &parent) const {
	Q_UNUSED(parent)

	if (row >= rowCount(parent) || column >= columnCount(parent)) {
		return QModelIndex();
	}

	if (row >= 0) {
		return createIndex(row, column, const_cast<Item *>(&items_[row]));
	} else {
		return createIndex(row, column);
	}
}

QModelIndex FilesystemModel::parent(const QModelIndex &index) const {
	Q_UNUSED(index)
	return QModelIndex();
}

QVariant FilesystemModel::data(const QModelIndex &index, int role) const {

	if (index.isValid()) {

		const Item &item = items_[index.row()];

		if (role == Qt::DisplayRole) {
			switch (index.column()) {
			case 0:
				return QVariant::fromValue(item.filename);

			}
		} else if (role == Qt::UserRole) {
			return QVariant::fromValue(item.path);
		}
	}

	return QVariant();
}

QVariant FilesystemModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		switch (section) {
		case 0:
			return tr("Filename");
		}
	}

	return QVariant();
}

int FilesystemModel::columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent)
	return 1;
}

int FilesystemModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent)
	return items_.size();
}

void FilesystemModel::addFile(const Item &item) {
	beginInsertRows(QModelIndex(), rowCount(), rowCount());


	items_.push_back(item);
	endInsertRows();
}

void FilesystemModel::clear() {
	beginResetModel();
	items_.clear();
	endResetModel();
}

void FilesystemModel::sort(int column, Qt::SortOrder order) {

	if (order == Qt::AscendingOrder) {
		switch (column) {
		case 0:
			std::sort(items_.begin(), items_.end(), [](const Item &s1, const Item &s2) { return s1.filename < s2.filename; });
			break;
		}
	} else {
		switch (column) {
		case 0:
			std::sort(items_.begin(), items_.end(), [](const Item &s1, const Item &s2) { return s1.filename > s2.filename; });
			break;
		}
	}

	Q_EMIT dataChanged(createIndex(0, 0, nullptr), createIndex(-1, -1, nullptr));
}
