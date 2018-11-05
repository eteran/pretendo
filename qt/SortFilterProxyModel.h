
#ifndef SORT_FILTER_PROXY_MODEL_20110419_H_
#define SORT_FILTER_PROXY_MODEL_20110419_H_

#include <QSortFilterProxyModel>

class SortFilterProxyModel final : public QSortFilterProxyModel {
	Q_OBJECT
public:
	explicit SortFilterProxyModel(QObject *parent = nullptr);

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif
