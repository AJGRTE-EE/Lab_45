#include "QWidget"
#include "dvninputdialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QValidator>
#include <qboxlayout.h>
#include <qformlayout.h>
#include <qlabel.h>
DVNINPUTDIALOG::DVNINPUTDIALOG(QWidget *parent, QStringList items)
        : QDialog(parent) {
    label1 = new QLabel("Введите удалённость от вершины");
    label2 = new QLabel("Выберете вершину");
    Combobox = new QComboBox(this);
    lineEdit = new QLineEdit;
    Combobox->addItems(items);
    //lineEdit->setValidator(new QValidator(0, 100, this));
    label1->setBuddy(lineEdit);
    label2->setBuddy(Combobox);

    buttonBox = new QDialogButtonBox
            (QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
             Qt::Horizontal, this);
    bool conn = connect(buttonBox, &QDialogButtonBox::accepted,
                        this, &DVNINPUTDIALOG::accept);
    Q_ASSERT(conn);
    conn = connect(buttonBox, &QDialogButtonBox::rejected,
                   this, &DVNINPUTDIALOG::reject);
    Q_ASSERT(conn);


    auto topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(label1);
    topLeftLayout->addWidget(lineEdit);

    auto midLeftLayout = new QHBoxLayout;
    midLeftLayout->addWidget(label2);
    midLeftLayout->addWidget(Combobox);


    auto leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);

    auto mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addLayout(leftLayout, 0, 0);
     mainLayout->addLayout(midLeftLayout,1,0);
    mainLayout->addWidget(buttonBox, 2, 0);

    mainLayout->setRowStretch(4, 2);

    setLayout(mainLayout);

    setWindowTitle(tr("Создание графа"));
}

std::string DVNINPUTDIALOG::initDVN(QWidget *parent, QStringList items, int &radix)
{
    auto dialog = new DVNINPUTDIALOG(parent, items);
    const int res = dialog->exec();
    QString result = {};
    if(res)
    {
        radix = dialog->lineEdit->text().toInt()+1;
        result = dialog->Combobox->currentText();
    }
    return result.toStdString();
}






/*void DVNINPUTDIALOG::init(QWidget *parent, bool &weighted, bool &directed, int &node_num, bool &ok) {
    auto dialog = new DVNINPUTDIALOG(parent);
    const int res = dialog->exec();
    if (res) {
        ok = res != 0;
        weighted = false;
        directed = false;
        node_num = dialog->lineEdit->text().toInt();
    }
    dialog->deleteLater();
}*/
