
#ifndef SORTFILTERPROXYMODEL_20110419_H_
#define SORTFILTERPROXYMODEL_20110419_H_

#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel {
	Q_OBJECT
public:
	SortFilterProxyModel(QObject *parent = nullptr);

protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

#endif
