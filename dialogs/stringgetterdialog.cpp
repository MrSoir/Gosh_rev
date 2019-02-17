#include "stringgetterdialog.h"

#include "staticfunctions_files.h"

StringGetterDialog::StringGetterDialog(const QString &headline,
                                       const QString &message,
                                       const QString& invalidInput_warning_message,
                                       const QString &initInput,
                                       std::function<bool (const QString& s)> stringValidator,
                                       QWidget *parent)
    : QDialog(parent),
      m_headline(headline),
      m_message(message),
      m_initInput(initInput),
      m_invalidInput_warning_message(invalidInput_warning_message),
      m_stringValidator(stringValidator)
{
//    this->moveToThread(QApplication::instance()->thread());

    createMainLayout();

    this->setModal(true);

    StaticFunctions::setIconToWidget(this);

    resize(400, 200);
}

void StringGetterDialog::stringSelected_SLOT()
{
    if(m_lineEdit && isValid(m_lineEdit->text()))
    {
        emit stringSelected(m_lineEdit->text());
    }else{
        emit stringSelected(QString(""));
    }
    closeDialog();
}

void StringGetterDialog::cancelClicked()
{
    emit stringSelected(QString(""));
    closeDialog();
}

void StringGetterDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    cancelClicked();
//    QDialog::closeEvent(event);
}

void StringGetterDialog::createMainLayout()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();

    QLabel* headline = new QLabel(m_headline);
    QLabel* message = new QLabel(m_message);
    QLabel* warningMsg = new QLabel(m_invalidInput_warning_message);
    warningMsg->setVisible(m_stringValidator(m_initInput));

    QLineEdit* le = new QLineEdit();
    le->setText(m_initInput);

    QPushButton* okBtn = new QPushButton("ok");
    QPushButton* cnclBtn = new QPushButton("cancel");

    connect(okBtn,   &QPushButton::clicked, this, &StringGetterDialog::stringSelected_SLOT);
    connect(cnclBtn, &QPushButton::clicked, this, &StringGetterDialog::cancelClicked);

    connect(le, &QLineEdit::textChanged, [=](const QString& newText){
        bool valid = isValid(newText);
        okBtn->setEnabled( valid );
        warningMsg->setVisible( !valid );
    });

    bool valid = isValid(m_initInput);
    okBtn->setEnabled( valid );
    warningMsg->setVisible( !valid );

    QHBoxLayout* btnLyt = new QHBoxLayout();
    btnLyt->addWidget(okBtn);
    btnLyt->addWidget(cnclBtn);

    mainLayout->addWidget(headline);
    mainLayout->addWidget(message);
    mainLayout->addWidget(le);
    mainLayout->addWidget(new QRubberBand(QRubberBand::Shape::Line));
    mainLayout->addWidget(warningMsg);
    mainLayout->addWidget(new QRubberBand(QRubberBand::Shape::Line));
    mainLayout->addLayout(btnLyt);

    this->setLayout(mainLayout);
}

void StringGetterDialog::closeDialog()
{
    this->close();
//    this->deleteLater();
}

bool StringGetterDialog::isValid(const QString &s)
{
    return !s.isEmpty() && m_stringValidator(s);
}

//----------------------------------------------------------------

StringGetterDialogCreator::StringGetterDialogCreator()
    : QObject(nullptr)
{
}

StringGetterDialogCreator::~StringGetterDialogCreator()
{
}

StringGetterDialog* StringGetterDialogCreator::createDialog(const QString &headline,
                                                            const QString &message,
                                                            const QString &invalidInput_warning_message,
                                                            const QString &initInput,
                                                            std::function<bool (const QString &)> stringValidator)
{
    return new StringGetterDialog(headline,
                                  message,
                                  invalidInput_warning_message,
                                  initInput,
                                  stringValidator);
}
