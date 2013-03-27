
#include "SortFilterProxyModel.h"
#include <QFileSystemModel>

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {

}

bool SortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {


	QAbstractItemModel *model = sourceModel();
	if(QFileSystemModel *const fs = qobject_cast<QFileSystemModel*>(model)) {
		const QModelIndex rootPathIndex = fs->index(fs->rootPath());
		if(rootPathIndex.isValid()) {
			QModelIndex source_parent_index = source_parent;
			while(source_parent_index.isValid()) {
				if(source_parent_index == rootPathIndex) {
					return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
				}

				source_parent_index = source_parent_index.parent();

			}
			return true;
		}
	}

	return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
