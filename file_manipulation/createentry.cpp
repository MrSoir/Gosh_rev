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
        COPY_MOVE::CopyFiles::makeDir(absPath);
    }else if(m_type == CREATION_TYPE::CREATE_FILE)
    {
        COPY_MOVE::CopyFiles::makeDir(absPath);
    }else{
        throw "CreateEntry::createEntry: not supported type selected!";
    }

}
