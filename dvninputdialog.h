#ifndef DVNINPUTDIALOG_H
#define DVNINPUTDIALOG_H

#include <QComboBox>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QCheckBox;

class QDialogButtonBox;

class QGroupBox;

class QLabel;

class QLineEdit;

class QPushButton;

QT_END_NAMESPACE

class DVNINPUTDIALOG : public QDialog {
Q_OBJECT

public:
    explicit DVNINPUTDIALOG(QWidget *parent = nullptr, QStringList items = {});

    static std::string initDVN(QWidget *parent, QStringList items, int &radix);

private:
    QLabel *label1;
    QLabel *label2;
    QLineEdit *lineEdit;
    QStringList items;
    QDialogButtonBox *buttonBox;
    QComboBox *Combobox;
};


#endif // DVNINPUTDIALOG_H
