#include <QtWidgets>
#include "widgets/headers/GraphOptionDialog.h"

GraphOptionDialog::GraphOptionDialog(QWidget *parent)
        : QDialog(parent) {
    label = new QLabel(tr("Введите количество вершин"));
    lineEdit = new QLineEdit;
    lineEdit->setValidator(new QIntValidator(0, 100, this));
    label->setBuddy(lineEdit);

    buttonBox = new QDialogButtonBox
            (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
             Qt::Horizontal, this);
    bool conn = connect(buttonBox, &QDialogButtonBox::accepted,
                        this, &GraphOptionDialog::accept);
    Q_ASSERT(conn);
    conn = connect(buttonBox, &QDialogButtonBox::rejected,
                   this, &GraphOptionDialog::reject);
    Q_ASSERT(conn);

    auto topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(label);
    topLeftLayout->addWidget(lineEdit);

    auto leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);

    auto mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addLayout(leftLayout, 0, 0);
    mainLayout->addWidget(buttonBox, 1, 0);
    mainLayout->setRowStretch(2, 1);

    setLayout(mainLayout);

    setWindowTitle(tr("Создание графа"));
}

void GraphOptionDialog::initGraph(QWidget *parent, bool &weighted, bool &directed, int &node_num, bool &ok) {
    auto dialog = new GraphOptionDialog(parent);
    const int res = dialog->exec();
    if (res) {
        ok = res != 0;
        weighted = false;
        directed = false;
        node_num = dialog->lineEdit->text().toInt();
    }
    dialog->deleteLater();
}
