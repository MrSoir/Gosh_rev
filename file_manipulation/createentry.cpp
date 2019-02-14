#include "createentry.h"

#include "copyfiles.h"

CreateEntry::CreateEntry(const string &baseDir,
                         QString dialog_message,
                         QString dialog_init_text,
                         CREATION_TYPE type)
    : TextDialogWorker(dialog_message,
                       dialog_init_text),
      m_baseDir(baseDir),
      m_type(type)
{
    qDebug() << "CreateEntry - CREATE_FILE: " << (type == CREATION_TYPE::CREATE_FILE ? "CREATE_FILE" : "CREATE_FOLDER");
}

CreateEntry::CreateEntry()
    : TextDialogWorker("",
                       ""),
      m_baseDir(""),
      m_type(CREATION_TYPE::CREATE_FOLDER)
{
}

CreateEntry::CreateEntry(const CreateEntry &ce)
    : TextDialogWorker(ce),
      m_baseDir(ce.m_baseDir),
      m_type(ce.m_type)
{
}

CreateEntry &CreateEntry::operator=(const CreateEntry &ce)
{
    TextDialogWorker::operator=(ce);
    this->m_baseDir = ce.m_baseDir;
    this->m_type = ce.m_type;
    return *this;
}

CreateEntry::~CreateEntry()
{
}

void CreateEntry::run()
{
    createEntry();
    finish();
}

std::function<bool (const QString &txt)> CreateEntry::getValidator()
{
    if(STATIC_FUNCTIONS::LOCAL_OS() == OS::WINDOWS)
    {
        return [](const QString& txt)
        {
            if(txt.isEmpty())
                return false;

            // in Windows darf ein dateiname nicht eine einfache 0 (NULL) sein laut api
            // mal ueberpruefen...
            if(txt.size() == 1 && txt.at(0) == '0')
                return false;

            // in windows wird der Punkt als file-extension-identifier verwendet,
            // daher darf ein fileName nicht mit einem Punkt enden:
            if(txt[txt.size()-1] == '.')
                return false;

            return !txt.contains('\\') &&
                   !txt.contains('<') &&
                   !txt.contains('>') &&
                   !txt.contains(':') &&
                   !txt.contains('"') &&
                   !txt.contains('/') &&
                   !txt.contains('?') &&
                   !txt.contains('*');
        };
    }else if(STATIC_FUNCTIONS::LOCAL_OS() == OS::LINUX)
    {
        return [](const QString& txt)
        {
            if(txt.isEmpty())
                return false;

            if(txt.size() == 1 && txt.at(0) == '0')
                return false;

            // in Linux darf ein dateiname kein backslash enthalten. alles andere scheint ok,
            // sofern in filename in quotes gesetzt wird. das sollte QT hoffentlich auch tunlichst so
            // handhaben... ein FileName wie z.B. "fileName-$%&|\.pdf" ist also generell vollkommen legitim:
            return !txt.contains('/');
        };
    }else{
        throw "CreateEntry::getValidator -> unsorrported os!";
    }
}

void CreateEntry::createEntry()
{
    QString q_baseDir = QString::fromStdString(m_baseDir);
    QString absPath = PATH::join(q_baseDir, m_receivedTextFromDialog);

    if(m_type == CREATION_TYPE::CREATE_FOLDER)
    {
        STATIC_FUNCTIONS::createNewFolder(absPath);
    }else if(m_type == CREATION_TYPE::CREATE_FILE)
    {
        STATIC_FUNCTIONS::createNewFile(absPath);
    }else{
        qDebug() << "CreateEntry::createEntry: not supported type selected!";
    }

}
