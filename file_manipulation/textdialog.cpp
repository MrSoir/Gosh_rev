#include "textdialog.h"

TextDialog::TextDialog(const QString &msg,
                       const QString& initLineEditMsg,
                       TextDialogWorker *worker,
                       std::function<bool(const QString&)> textValidator,
                       QWidget *parent)
    : ThreadedDialog(worker, parent),
      m_textValidator(textValidator),
      m_msg(new QLabel(this)),
      m_validatorLbl(new QLabel(this)),
      m_lineEdit(new QLineEdit(this)),
      m_textIsValid(true)
{
    m_msg->setText(msg);
    m_lineEdit->setText(initLineEditMsg);
    validateText();
    setLayout();

    connectSignals();
    emit startThread();

    this->show();
}

TextDialog::~TextDialog()
{
}

void TextDialog::okClicked()
{
    emit textSelected(m_lineEdit->text());

    // kein TextDialog::close() -> mit emit textSelected wird TextDialogWorker::run() gestartet
    // => damit beginnt der TextDialogWorker seine arbeit. wenn er fertig ist, sendet er das signal
    // TextDialogWorker::finished() -> das wiederum schliesst den TextDialog!
    okBtn->setEnabled(false);
    cnclBtn->setEnabled(false);
}

void TextDialog::cancelClicked()
{
    this->close(); // close-event wird in parent-class dazu fuehren, dass cancel gesendet wird -> worker wird gecancelled und stirbt damit. da der worker nicht aktiv laeuft wird er damit auch gleich deleted!
}

void TextDialog::validateText()
{

    m_textIsValid = m_textValidator(m_msg->text());

    if(m_textIsValid)
    {
        m_validatorLbl->setText("");
    }else
    {
        m_validatorLbl->setText("invalid input!");
    }
}

void TextDialog::connectSignals()
{
    TextDialogWorker* worker = static_cast<TextDialogWorker*>(m_worker);
    connect(this, &TextDialog::textSelected, worker, &TextDialogWorker::textSelected);
}

void TextDialog::setLayout()
{
    m_msg->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    QHBoxLayout* lineEditLyt = new QHBoxLayout(this);
    lineEditLyt->addWidget(m_msg);
    lineEditLyt->addWidget(m_lineEdit);

    QPushButton* okBtn = new QPushButton("ok");
    QPushButton* cnclBtn = new QPushButton("cancel");

    QHBoxLayout* btnLyt = new QHBoxLayout();
    btnLyt->addWidget(okBtn);
    btnLyt->addWidget(cnclBtn);

    connect(okBtn,   &QPushButton::clicked, this, &TextDialog::okClicked);
    connect(cnclBtn, &QPushButton::clicked, this, &TextDialog::cancelClicked);

    connect(m_lineEdit, &QLineEdit::textChanged, this, &TextDialog::validateText);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(lineEditLyt);
    mainLayout->addWidget(m_validatorLbl);
    mainLayout->addLayout(btnLyt);

    QWidget::setLayout(mainLayout);
}
